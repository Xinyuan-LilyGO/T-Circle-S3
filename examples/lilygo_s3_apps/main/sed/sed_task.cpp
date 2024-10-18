#include "sed_task.h"
#include "audio_preprocessor.h"
#include "i2s_rx_slot.h"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "freertos/ringbuf.h"
#include "freertos/semphr.h"
#include "freertos/stream_buffer.h"
#include "freertos/task.h"

#include "esp_agc.h"
#include "esp_log.h"
#include "esp_timer.h"

static const char *TAG = "sed_task";

#define SED_EVENT_START_MSK BIT0
#define SED_EVENT_STOP_MSK  BIT1
#define SED_STATUS_BUSY_MSK BIT2

#define MFCC_DATA_FRAME_SZ  (SED_NUM_FBANK_BINS * sizeof(float))
#define MFCC_DATA_BUFFER_SZ (MFCC_DATA_FRAME_SZ * SED_FRAME_NUM)

#define SED_FRAME_SZ          (SED_FRAME_LEN * MIC_ELEM_BYTES)
#define SED_FRAME_SHIFT_BYTES (SED_FRAME_SHIFT * MIC_ELEM_BYTES)

#define AGC_FRAME_LEN_MS 10
#define AGC_FRAME_LEN    (CONFIG_MIC_SAMPLE_RATE / 1000 * AGC_FRAME_LEN_MS)

#define SED_WINDOW  3
#define REQ_CAT_IDX 2

QueueHandle_t xSEDResultQueue = NULL;
static StreamBufferHandle_t xSEDFramesBuffer = NULL;
static EventGroupHandle_t xSEDEventGroup = NULL;

static void *s_agc_handle = NULL;

static TaskHandle_t xPPTaskHandle = NULL;
static TaskHandle_t xSEDTaskHandle = NULL;
static AudioPreprocessor *pp = NULL;

static void read_frame(audio_t *dst) {
  raw_audio_t raw_data_buffer[MIC_FRAME_LEN] = {0};

  i2s_rx_slot_read(raw_data_buffer, sizeof(raw_data_buffer), portMAX_DELAY);

  for (size_t i = 0; i < MIC_FRAME_LEN; i++) {
    dst[i] = audio_t(raw_data_buffer[i] >> 16);
  }
}

static void pp_task(void *pv) {
  AudioPreprocessor *preprocessor = static_cast<AudioPreprocessor *>(pv);
  audio_t proc_frame[SED_FRAME_LEN] = {0};
  float mfcc_buffer[SED_FEATURES_LEN] = {0};

  audio_t *proc_buf = &proc_frame[0];
  audio_t *half_proc_buf = &proc_frame[SED_FRAME_SHIFT];

  for (size_t i = 0; i < SED_FRAME_SHIFT / AGC_FRAME_LEN; i++) {
    audio_t *ptr = &proc_buf[i * AGC_FRAME_LEN];
    read_frame(ptr);
    esp_agc_process(s_agc_handle, ptr, ptr, AGC_FRAME_LEN,
                    CONFIG_MIC_SAMPLE_RATE);
  }

  size_t frame_counter = 0;
  for (;;) {
    for (size_t i = 0; i < SED_FRAME_SHIFT / AGC_FRAME_LEN; i++) {
      audio_t *ptr = &half_proc_buf[i * AGC_FRAME_LEN];
      read_frame(ptr);
      esp_agc_process(s_agc_handle, ptr, ptr, AGC_FRAME_LEN,
                      CONFIG_MIC_SAMPLE_RATE);
    }

    const int64_t t1 = esp_timer_get_time();

    const size_t current_frame = frame_counter % SED_FRAME_NUM;

    preprocessor->LogMelCompute(
      proc_frame, &mfcc_buffer[current_frame * SED_NUM_FBANK_BINS], 1 << 15);

    memmove(proc_frame, half_proc_buf, SED_FRAME_SHIFT_BYTES);
    memset(half_proc_buf, 0, SED_FRAME_SHIFT_BYTES);

    ESP_LOGV(TAG, "pp_frame: %u(%u), %lld us", current_frame, frame_counter,
             esp_timer_get_time() - t1);

    if ((frame_counter + 1) >= SED_FRAME_NUM) {
      if (!(xEventGroupGetBits(xSEDEventGroup) & SED_STATUS_BUSY_MSK)) {
        for (size_t k = 1; k <= SED_FRAME_NUM; k++) {
          const size_t frame_num = (frame_counter + k) % SED_FRAME_NUM;
          void *frame_ptr =
            (void *)&mfcc_buffer[frame_num * SED_NUM_FBANK_BINS];

          const auto xBytesSent = xStreamBufferSend(xSEDFramesBuffer, frame_ptr,
                                                    MFCC_DATA_FRAME_SZ, 0);
          if (xBytesSent < MFCC_DATA_FRAME_SZ) {
            ESP_LOGW(TAG, "xSEDFramesBuffer: xBytesSent=%d (%d)", xBytesSent,
                     MFCC_DATA_FRAME_SZ);
          }
        }
        ESP_LOGV(TAG, "sent frames: [%d; %d]", frame_counter - SED_FRAME_NUM,
                 frame_counter);
      }
      ESP_LOGV(TAG, "skipped frame: %d", frame_counter);
    }

    frame_counter++;
  }
}

void sed_task(void *pv) {
  nn_model_handle_t model_handle = static_cast<nn_model_handle_t>(pv);
  float mfcc_buffer[SED_FEATURES_LEN] = {0};

  int cats_buffer[SED_WINDOW] = {-1};
  size_t num_det = 0;
  uint8_t trig = 0;
  i2s_rx_slot_start();
  for (size_t counter = 0;; counter++) {
    const auto xReceivedBytes = xStreamBufferReceive(
      xSEDFramesBuffer, mfcc_buffer, MFCC_DATA_BUFFER_SZ, portMAX_DELAY);
    ESP_LOGV(TAG, "recv bytes=%d", xReceivedBytes);

    xEventGroupSetBits(xSEDEventGroup, SED_STATUS_BUSY_MSK);
    int category = -1;
    if (nn_model_inference(model_handle, mfcc_buffer, SED_FEATURES_LEN,
                           &category) < 0) {
      ESP_LOGE(TAG, "inference error");
      continue;
    }
    num_det += category == REQ_CAT_IDX;
    cats_buffer[counter % SED_WINDOW] = category;

    if (!trig) {
      if (num_det == SED_WINDOW) {
        trig = 1;
        xQueueSend(xSEDResultQueue, &category, 0);
      }
    } else {
      if (num_det == 0) {
        trig = 0;
      }
    }
    num_det -= cats_buffer[(counter + 1) % SED_WINDOW] == REQ_CAT_IDX;

    xEventGroupClearBits(xSEDEventGroup, SED_STATUS_BUSY_MSK);
  }
}

int sed_task_init(sed_task_conf_t conf) {
  ESP_LOGD(TAG,
           "MFCC_DATA_FRAME_SZ=%d, MFCC_DATA_BUFFER_SZ=%d, SED_FEATURES_LEN=%d",
           MFCC_DATA_FRAME_SZ, MFCC_DATA_BUFFER_SZ, SED_FEATURES_LEN);

  s_agc_handle = esp_agc_open(3, CONFIG_MIC_SAMPLE_RATE);
  if (!s_agc_handle) {
    ESP_LOGE(TAG, "Unable to create agc");
    return -1;
  }
  set_agc_config(s_agc_handle, conf.mic_gain, 1, 0);

  xSEDFramesBuffer =
    xStreamBufferCreate(MFCC_DATA_BUFFER_SZ, MFCC_DATA_BUFFER_SZ);
  if (xSEDFramesBuffer == NULL) {
    ESP_LOGE(TAG, "Error creating sed frames buffer");
    return -1;
  }
  xSEDResultQueue = xQueueCreate(1, sizeof(int));
  if (xSEDResultQueue == NULL) {
    ESP_LOGE(TAG, "Error creating SED result queue");
    return -1;
  }

  xSEDEventGroup = xEventGroupCreate();
  if (xSEDEventGroup == NULL) {
    ESP_LOGE(TAG, "Error creating xSEDEventGroup");
    return -1;
  }

  pp = new AudioPreprocessor(10, SED_FRAME_LEN, SED_NUM_FBANK_BINS,
                             SED_MEL_LOW_FREQ, SED_MEL_HIGH_FREQ);
  auto xReturned =
    xTaskCreate(pp_task, "pp_task", configMINIMAL_STACK_SIZE + 1024 * 10, pp, 1,
                &xPPTaskHandle);
  if (xReturned != pdPASS) {
    ESP_LOGE(TAG, "Error creating pp_task");
    return -1;
  }
  xReturned =
    xTaskCreate(sed_task, "sed_task", configMINIMAL_STACK_SIZE + 1024 * 10,
                conf.model_handle, 1, &xSEDTaskHandle);
  if (xReturned != pdPASS) {
    ESP_LOGE(TAG, "Error creating sed_task");
    return -1;
  }
  return 0;
}

void sed_task_release() {
  i2s_rx_slot_stop();

  if (s_agc_handle) {
    esp_agc_close(s_agc_handle);
    s_agc_handle = NULL;
  }
  if (xSEDFramesBuffer) {
    vStreamBufferDelete(xSEDFramesBuffer);
    xSEDFramesBuffer = NULL;
  }
  if (xSEDResultQueue) {
    vQueueDelete(xSEDResultQueue);
    xSEDResultQueue = NULL;
  }
  if (xSEDEventGroup) {
    vEventGroupDelete(xSEDEventGroup);
    xSEDEventGroup = NULL;
  }
  if (xPPTaskHandle) {
    vTaskDelete(xPPTaskHandle);
    xPPTaskHandle = NULL;
  }
  if (xSEDTaskHandle) {
    vTaskDelete(xSEDTaskHandle);
    xSEDTaskHandle = NULL;
  }
  if (pp) {
    delete pp;
  }
}
