#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#include "esp_log.h"

enum eEvent : unsigned {
  NO_EVENT = 0,
  TOUCH_CLICK,
  TOUCH_SWIPE_RIGHT,
  TOUCH_SWIPE_LEFT,
  KWS_WORD,
  TIMEOUT,
};

/*!
 * \brief Global timer.
 */
extern TimerHandle_t xTimer;
/*!
 * \brief Global events queue.
 */
extern QueueHandle_t xEventQueue;
/*!
 * \brief Initialize Events Generator.
 * \return Result.
 */
int initEventsGenerator();
/*!
 * \brief Release Events Generator.
 */
void releaseEventsGenerator();
/*!
 * \brief Send event to events queue.
 * \param ev Event.
 */
void sendEvent(eEvent ev);
/*!
 * \brief Convert event to string.
 * \return String representation.
 */
const char *event_to_str(eEvent ev);
