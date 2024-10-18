#ifndef _DEF_H_
#define _DEF_H_

#include "stddef.h"
#include "stdint.h"

#include "driver/gpio.h"

#if CONFIG_TARGET_LILYGO_T_CIRCLE
#define MIC_BCLK_PIN GPIO_NUM_7
#define MIC_WS_PIN   GPIO_NUM_9
#define MIC_DATA_PIN GPIO_NUM_8
#else
#error "unknown target"
#endif

typedef int32_t raw_audio_t;
typedef int16_t audio_t;
#define MIC_FRAME_LEN_MS 10
#define MIC_ELEM_BYTES   sizeof(audio_t)
#define MIC_FRAME_LEN    (CONFIG_MIC_SAMPLE_RATE / 1000 * MIC_FRAME_LEN_MS)
#define MIC_FRAME_SZ     (MIC_FRAME_LEN * MIC_ELEM_BYTES)

#define I2S_RX_PORT_NUMBER I2S_NUM_1

#endif // _DEF_H_
