#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/portmacro.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "ILed.hpp"

#define GET_BIT_POS(mask) 31 - __builtin_clz(mask)
#define ALL_BITS(pos)     (BIT##pos - 1)

#define STATUS_STATE_UNLOCKED_MSK  BIT0
#define STATUS_EVENT_KWS_WORD_MSK  BIT1
#define STATUS_EVENT_GOOD_MSK      BIT2
#define STATUS_EVENT_BAD_MSK       BIT3
#define STATUS_STATE_BAD_MSK       BIT4
#define STATUS_STATE_SUSPENDED_MSK BIT5
#define STATUS_ALL_BITS_MSK        ALL_BITS(6)
#define STATUS_EVENT_BITS_MSK                                                  \
  (STATUS_EVENT_GOOD_MSK | STATUS_EVENT_BAD_MSK | STATUS_EVENT_KWS_WORD_MSK)

/*!
 * \brief Global status bits.
 */
extern EventGroupHandle_t xStatusEventGroup;
/*!
 * \brief Initialize Status Monitor.
 * \param p_led Pointer to led device.
 * \return Result.
 */
int initStatusMonitor(ILed *p_led);
/*!
 * \brief Release Status Monitor.
 */
void releaseStatusMonitor();
