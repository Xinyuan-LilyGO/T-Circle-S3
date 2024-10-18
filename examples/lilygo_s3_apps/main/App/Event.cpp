#include "Event.hpp"

static constexpr char TAG[] = "Event";

TimerHandle_t xTimer = NULL;
QueueHandle_t xEventQueue = NULL;

void sendEvent(eEvent ev) {
  if (xQueueSend(xEventQueue, &ev, 0) == pdPASS) {
    ESP_LOGD(TAG, "send ev=%s", event_to_str(ev));
  } else {
    ESP_LOGD(TAG, "skip ev=%s", event_to_str(ev));
  }
}

static void vTimerCallback(TimerHandle_t xTimer) {
  ESP_LOGD(TAG, "send timeout event");
  sendEvent(eEvent::TIMEOUT);
}

int initEventsGenerator() {
  xEventQueue = xQueueCreate(5, sizeof(eEvent));
  if (xEventQueue == NULL) {
    ESP_LOGE(TAG, "Error creating event queue");
    return -1;
  }
  xTimer =
    xTimerCreate("Timer", pdMS_TO_TICKS(1000), pdFALSE, NULL, vTimerCallback);
  if (xTimer == NULL) {
    ESP_LOGE(TAG, "Error creating xTimer");
    return -1;
  } else {
    xTimerStop(xTimer, 0);
  }

  return 0;
}

void releaseEventsGenerator() {
  if (xEventQueue) {
    vQueueDelete(xEventQueue);
    xEventQueue = NULL;
  }
  if (xTimer) {
    xTimerStop(xTimer, 0);
    xTimerDelete(xTimer, 0);
    xTimer = NULL;
  }
}

const char *event_to_str(eEvent ev) {
  switch (ev) {
  case eEvent::NO_EVENT:
    return "NO_EVENT";
    break;
  case eEvent::TOUCH_CLICK:
    return "TOUCH_CLICK";
    break;
  case eEvent::TOUCH_SWIPE_LEFT:
    return "TOUCH_SWIPE_LEFT";
    break;
  case eEvent::TOUCH_SWIPE_RIGHT:
    return "TOUCH_SWIPE_RIGHT";
    break;
  case eEvent::KWS_WORD:
    return "KWS_WORD";
    break;
  case eEvent::TIMEOUT:
    return "TIMEOUT";
    break;
  default:
    return "";
  }
}
