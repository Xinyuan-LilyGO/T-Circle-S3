#ifndef _SED_TASK_H_
#define _SED_TASK_H_

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "nn_model.h"

#define SED_NUM_FBANK_BINS 40
#define SED_MEL_LOW_FREQ   0
#define SED_MEL_HIGH_FREQ  8000

#define SED_WIN_MS      40
#define SED_STRIDE_MS   20
#define SED_DURATION_MS 1000
#define SED_FRAME_LEN   (CONFIG_MIC_SAMPLE_RATE / 1000 * SED_WIN_MS)
#define SED_FRAME_SHIFT (CONFIG_MIC_SAMPLE_RATE / 1000 * SED_STRIDE_MS)
#define SED_FRAME_NUM   ((SED_DURATION_MS - SED_WIN_MS) / SED_STRIDE_MS + 1)

#define SED_FEATURES_LEN SED_FRAME_NUM *SED_NUM_FBANK_BINS

/*! \brief Global SED result queue. */
extern QueueHandle_t xSEDResultQueue;

struct sed_task_conf_t {
  nn_model_handle_t model_handle;
  int mic_gain;
};

/*!
 * \brief Initialize SED task.
 * \param conf Configuration params.
 * \return Result.
 */
int sed_task_init(sed_task_conf_t conf);
/*!
 * \brief Release SED task.
 */
void sed_task_release();

#endif // _SED_TASK_H_
