#include "Event.hpp"

#include "kws_event_task.h"
#include "kws_task.h"

#include "esp_log.h"

static const char *TAG = "KWS";

static TaskHandle_t s_task_handle = NULL;

static void kws_event_task(void *pv) {
  for (;;) {
    if (xSemaphoreTake(xKWSSema, portMAX_DELAY) == pdPASS) {
      sendEvent(eEvent::KWS_WORD);
    }
  }
}

int kws_event_task_init() {
  auto xReturned = xTaskCreate(kws_event_task, "kws_event_task",
                               configMINIMAL_STACK_SIZE + 1024, NULL,
                               tskIDLE_PRIORITY, &s_task_handle);
  if (xReturned != pdPASS) {
    ESP_LOGE(TAG, "Error creating KWS event task");
    return -1;
  }

  return 0;
}

void kws_event_task_release() {
  if (s_task_handle) {
    vTaskDelete(s_task_handle);
    s_task_handle = NULL;
  }
}
