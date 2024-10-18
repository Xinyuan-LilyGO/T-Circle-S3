#include "Touch.hpp"
#include "Arduino_DriveBus_Library.h"
#include "Event.hpp"
#include "freertos/projdefs.h"

// device
#define TP_SDA 11
#define TP_SCL 14
#define TP_RST -1
#define TP_INT 12

// IIC
#define IIC_SDA 11
#define IIC_SCL 14

#define TOUCH_CLICK_DURATION_MS       100
#define TOUCH_SWIPE_DELAY_DURATION_MS 300

static constexpr char TAG[] = "Touch";

static std::shared_ptr<Arduino_IIC_DriveBus> IIC_Bus;
static Arduino_IIC *device;

void Arduino_IIC_Touch_Interrupt(void) { device->IIC_Interrupt_Flag = true; }

static TaskHandle_t s_task_handle = NULL;

size_t getTimeMS() { return xTaskGetTickCount() * portTICK_PERIOD_MS; }

static void touch_event_task(void *pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();

  size_t click_time = 0;
  bool pendind_click = 0;

  for (;;) {
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));

    if (device->IIC_Interrupt_Flag == true) {
      const auto gesture = device->IIC_Read_Device_State(
        device->Arduino_IIC_Touch::Status_Information::TOUCH_GESTURE_ID);
      ESP_LOGV(TAG, "Gesture: %s", gesture.c_str());
      if (strcmp(gesture.c_str(), "Swipe Left") == 0) {
        sendEvent(eEvent::TOUCH_SWIPE_LEFT);
        vTaskDelay(pdMS_TO_TICKS(TOUCH_SWIPE_DELAY_DURATION_MS));
        pendind_click = 0;
      } else if (strcmp(gesture.c_str(), "Swipe Right") == 0) {
        sendEvent(eEvent::TOUCH_SWIPE_RIGHT);
        vTaskDelay(pdMS_TO_TICKS(TOUCH_SWIPE_DELAY_DURATION_MS));
        pendind_click = 0;
        // } else if (strcmp(gesture.c_str(), "Single Click") == 0) {
        //   sendEvent(eEvent::TOUCH_CLICK);
      } else { // NONE
        if (!pendind_click) {
          pendind_click = 1;
          click_time = getTimeMS();
        }
      }

      device->IIC_Interrupt_Flag = false;
    } else {
      if (pendind_click) {
        const auto cur_time = getTimeMS();
        if (cur_time > click_time + TOUCH_CLICK_DURATION_MS) {
          pendind_click = 0;
          sendEvent(eEvent::TOUCH_CLICK);
        }
      }
    }
  }
}

int initTouch() {
  IIC_Bus = std::make_shared<Arduino_HWIIC>(IIC_SDA, IIC_SCL, &Wire);
  device = new Arduino_CST816x(IIC_Bus, CST816D_DEVICE_ADDRESS, TP_RST, TP_INT,
                               Arduino_IIC_Touch_Interrupt);
  if (device->begin() == false) {
    ESP_LOGI(TAG, "CST816D initialization fail");
    return -1;
  }

  device->IIC_Write_Device_State(
    device->Arduino_IIC_Touch::Device::TOUCH_DEVICE_INTERRUPT_MODE,
    device->Arduino_IIC_Touch::Device_Mode::TOUCH_DEVICE_INTERRUPT_PERIODIC);

  auto xReturned = xTaskCreate(touch_event_task, "touch_event_task",
                               configMINIMAL_STACK_SIZE + 1024 * 2, NULL,
                               tskIDLE_PRIORITY, &s_task_handle);
  if (xReturned != pdPASS) {
    ESP_LOGE(TAG, "Error creating button event task");
    return -1;
  }
  return 0;
}

void releaseTouch() {
  IIC_Bus.reset();
  delete device;

  if (s_task_handle) {
    vTaskDelete(s_task_handle);
    s_task_handle = NULL;
  }
}
