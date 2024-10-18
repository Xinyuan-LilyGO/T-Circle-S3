#include "Status.hpp"
#include "ILed.hpp"

#include "i2s_rx_slot.h"

#include "esp_log.h"

static constexpr char TAG[] = "Status";

static TaskHandle_t s_task_handle = NULL;
EventGroupHandle_t xStatusEventGroup = NULL;

static void status_monitor_task(void *pvParameters) {
  auto *p_led = static_cast<ILed *>(pvParameters);
  const TickType_t xTicks = 50 / portTICK_PERIOD_MS;
  TickType_t xEnterTime[GET_BIT_POS(STATUS_ALL_BITS_MSK) + 1] = {0};

  auto blinker = [p_led, xTicks, &xEnterTime](
                   EventBits_t mask, TickType_t timeout, ILed::Colour colour,
                   unsigned hold_time, unsigned led_num = -1,
                   ILed::Brightness b = ILed::Brightness::_50) mutable {
    auto &time = xEnterTime[GET_BIT_POS(mask)];
    time += xTicks;
    if (time > timeout) {
      p_led->set(colour, led_num, b);
      vTaskDelay(pdMS_TO_TICKS(hold_time));
      time = 0;
    } else {
      p_led->set(ILed::Black, led_num);
    }
  };

  for (;;) {
    const auto xBits = xEventGroupWaitBits(
      xStatusEventGroup, STATUS_EVENT_BITS_MSK, pdTRUE, pdFALSE, xTicks);
    if (xBits & STATUS_EVENT_BITS_MSK) {
      if (xBits & STATUS_EVENT_GOOD_MSK) {
        p_led->set(ILed::Green);
        vTaskDelay(pdMS_TO_TICKS(800));
      } else if (xBits & STATUS_EVENT_BAD_MSK) {
        p_led->set(ILed::Red);
        vTaskDelay(pdMS_TO_TICKS(800));
      } else if (xBits & STATUS_EVENT_KWS_WORD_MSK) {
        p_led->set(ILed::Green);
        vTaskDelay(pdMS_TO_TICKS(100));
      }
      p_led->set(ILed::Black);
    } else {
      const auto xMicBits = xEventGroupGetBits(xMicEventGroup);

      if (xBits & STATUS_STATE_UNLOCKED_MSK) {
        p_led->set(ILed::Green);
      } else if (xBits & STATUS_STATE_BAD_MSK) {
        blinker(STATUS_STATE_BAD_MSK, 800, ILed::Red, 200, -1);
      } else if (xBits & STATUS_STATE_SUSPENDED_MSK) {
        blinker(STATUS_STATE_SUSPENDED_MSK, 5000, ILed::Blue, 200, -1,
                ILed::Brightness::_25);
      } else if (xMicBits & MIC_ON_MSK) {
        p_led->set(ILed::Blue);
      } else {
        p_led->set(ILed::Black);
      }
    }
  }
}

int initStatusMonitor(ILed *p_led) {
  xStatusEventGroup = xEventGroupCreate();
  if (xStatusEventGroup == NULL) {
    ESP_LOGE(TAG, "Error creating xStatusEventGroup");
    return -1;
  }

  auto xReturned =
    xTaskCreate(status_monitor_task, "status_monitor_task",
                configMINIMAL_STACK_SIZE + 512, p_led, 2, &s_task_handle);
  if (xReturned != pdPASS) {
    ESP_LOGE(TAG, "Error creating task");
    return -1;
  }
  return 0;
}

void releaseStatusMonitor() {
  if (s_task_handle) {
    vTaskDelete(s_task_handle);
    s_task_handle = NULL;
  }
  if (xStatusEventGroup) {
    vEventGroupDelete(xStatusEventGroup);
    xStatusEventGroup = NULL;
  }
}
