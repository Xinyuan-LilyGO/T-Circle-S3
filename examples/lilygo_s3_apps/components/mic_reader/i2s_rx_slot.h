#ifndef _I2S_RX_SLOT_H_
#define _I2S_RX_SLOT_H_

#include "stddef.h"
#include "stdint.h"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/portmacro.h"
#include "freertos/semphr.h"

#define MIC_ON_MSK BIT0

#include "def.h"

/*! \brief Global microphone semaphore. */
extern SemaphoreHandle_t xMicSema;
/*! \brief Global microphone event bits. */
extern EventGroupHandle_t xMicEventGroup;

/*!
 * \brief Initialize microphone rx slot.
 */
void i2s_rx_slot_init();
/*!
 * \brief Enable microphone rx slot.
 */
void i2s_rx_slot_start();
/*!
 * \brief Stop microphone rx slot.
 */
void i2s_rx_slot_stop();
/*!
 * \brief Release microphone rx slot.
 */
void i2s_rx_slot_release();
/*!
 * \brief Read from rx slot.
 */
int i2s_rx_slot_read(void *buffer, size_t bytes, size_t timeout_ticks);

#endif // _I2S_RX_SLOT_H_
