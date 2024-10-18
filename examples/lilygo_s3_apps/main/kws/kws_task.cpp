#include "esp_agc.h"
#include "esp_log.h"
#include "esp_ns.h"
#include "esp_timer.h"
#include "esp_vad.h"

#include "audio_preprocessor.h"
#include "freertos/portmacro.h"
#include "freertos/projdefs.h"
#include "i2s_rx_slot.h"
#include "kws_task.h"

static const char *TAG = "kws_task";

QueueHandle_t xWordQueue = NULL;
StreamBufferHandle_t xWordFramesBuffer = NULL;
SemaphoreHandle_t xKWSSema = NULL;
QueueHandle_t xKWSRequestQueue = NULL;
QueueHandle_t xKWSResultQueue = NULL;
EventGroupHandle_t xKWSEventGroup = NULL;

static TaskHandle_t xVADTaskHandle = NULL;
static TaskHandle_t xKWSTaskHandle = NULL;

struct kws_task_param_t {
  nn_model_handle_t model_handle = NULL;
  AudioPreprocessor *pp = NULL;
} static s_kws_task_params;

static void *s_agc_handle = NULL;
static ns_handle_t s_ns_handle = NULL;
static vad_handle_t s_vad_handle = NULL;

#define DET_IS_VOICED_THRESHOLD       0.9
#define DET_VOICED_FRAMES_WINDOW      26
#define DET_VOICED_FRAMES_THRESHOLD   12
#define DET_UNVOICED_FRAMES_THRESHOLD 2

#define KWS_FRAME_SZ          (KWS_FRAME_LEN * MIC_ELEM_BYTES)
#define KWS_FRAME_SHIFT_BYTES (KWS_FRAME_SHIFT * MIC_ELEM_BYTES)
#define MFCC_BUF_SZ           (KWS_FRAME_NUM * KWS_NUM_MFCC * sizeof(float))
#define MFCC_PROC_FRAME_NUM   (KWS_FRAME_SHIFT / MIC_FRAME_LEN)

#define PROC_BUF_SZ        KWS_FRAME_SZ
#define PROC_BUF_FRAME_NUM (PROC_BUF_SZ / MIC_FRAME_SZ)

#define AGC_FRAME_LEN_MS 10
#define AGC_FRAME_LEN    (CONFIG_MIC_SAMPLE_RATE / 1000 * AGC_FRAME_LEN_MS)

static const float silence_mfcc_coeffs[KWS_NUM_MFCC] = {
  -247.13936,    8.881784e-16,   2.220446e-14,   -1.0658141e-14,
  8.881784e-16,  -1.5987212e-14, 1.15463195e-14, -4.440892e-15,
  1.0658141e-14, -4.7961635e-14};

static audio_t current_frames[DET_VOICED_FRAMES_WINDOW * MIC_FRAME_LEN] = {0};
static raw_audio_t raw_data_buffer[MIC_FRAME_LEN] = {0};

static size_t compute_max_abs(audio_t *data, size_t len) {
  size_t max_abs = 0;
  for (size_t j = 0; j < len; j++) {
    const size_t abs_val = abs(data[j]);
    if (abs_val > max_abs) {
      max_abs = abs_val;
    }
  }
  return max_abs;
}

static void vad_task(void *pv) {
  size_t max_abs_arr[DET_VOICED_FRAMES_WINDOW] = {0};
  uint8_t is_speech_arr[DET_VOICED_FRAMES_WINDOW] = {0};
  size_t cur_frame = 0;
  size_t num_voiced = 0;
  uint8_t trig = 0;
  WordDesc_t word;

  for (;;) {
    audio_t *proc_data =
      &current_frames[(cur_frame % DET_VOICED_FRAMES_WINDOW) * MIC_FRAME_LEN];
    if (i2s_rx_slot_read(raw_data_buffer, sizeof(raw_data_buffer),
                         portMAX_DELAY) < 0) {
      continue;
    }

    for (size_t i = 0; i < MIC_FRAME_LEN; i++) {
      proc_data[i] = audio_t(raw_data_buffer[i] >> 16);
    }

    esp_agc_process(s_agc_handle, proc_data, proc_data, AGC_FRAME_LEN,
                    CONFIG_MIC_SAMPLE_RATE);

    ns_process(s_ns_handle, proc_data, proc_data);

    const auto vad_res = vad_process(s_vad_handle, proc_data,
                                     CONFIG_MIC_SAMPLE_RATE, AGC_FRAME_LEN_MS);
    const uint8_t is_speech = vad_res == VAD_SPEECH;
    num_voiced += is_speech;
    is_speech_arr[cur_frame % DET_VOICED_FRAMES_WINDOW] = is_speech;

    const size_t max_abs = compute_max_abs(proc_data, MIC_FRAME_LEN);

    max_abs_arr[cur_frame % DET_VOICED_FRAMES_WINDOW] = max_abs;

    if (!trig) {
      if (num_voiced >= DET_VOICED_FRAMES_THRESHOLD) {
        word.frame_num = 0;
        word.max_abs = 0;
        trig = 1;
        ESP_LOGD(TAG, "__start[%d]=%d, max_abs=%d",
                 cur_frame - DET_VOICED_FRAMES_WINDOW, cur_frame, max_abs);
        for (size_t k = 1; k <= DET_VOICED_FRAMES_WINDOW; k++) {
          const size_t frame_num = (cur_frame + k) % DET_VOICED_FRAMES_WINDOW;
          audio_t *frame_ptr = &current_frames[frame_num * MIC_FRAME_LEN];
          const auto xBytesSent =
            xStreamBufferSend(xWordFramesBuffer, frame_ptr, MIC_FRAME_SZ, 0);
          if (xBytesSent < MIC_FRAME_SZ) {
            ESP_LOGW(TAG, "xWordFramesBuffer: xBytesSent=%d (%d)", xBytesSent,
                     MIC_FRAME_SZ);
          }
          word.frame_num++;
          word.max_abs = std::max(word.max_abs, max_abs_arr[frame_num]);
        }
      }
    } else {
      if (num_voiced <= DET_UNVOICED_FRAMES_THRESHOLD) {
        trig = 0;
        ESP_LOGD(TAG, "__end[%d]=%d, max_abs=%d",
                 cur_frame - DET_VOICED_FRAMES_WINDOW, cur_frame, max_abs);
        xQueueSend(xWordQueue, &word, 0);
      } else {
        word.frame_num++;
        word.max_abs = std::max(word.max_abs, max_abs);
        const auto xBytesSent =
          xStreamBufferSend(xWordFramesBuffer, proc_data, MIC_FRAME_SZ, 0);
        if (xBytesSent < MIC_FRAME_SZ) {
          ESP_LOGW(TAG, "xWordFramesBuffer: xBytesSent=%d (%d)", xBytesSent,
                   MIC_FRAME_SZ);
        }
      }
    }

    num_voiced -= is_speech_arr[(cur_frame + 1) % DET_VOICED_FRAMES_WINDOW];

    cur_frame++;
  }
}

void kws_task(void *pv) {
  kws_task_param_t *params = static_cast<kws_task_param_t *>(pv);
  AudioPreprocessor *preprocessor = params->pp;
  nn_model_handle_t model = params->model_handle;
  uint8_t proc_buf[PROC_BUF_SZ] = {0};
  float mfcc_buffer[KWS_FRAME_NUM * KWS_NUM_MFCC];

  xStreamBufferSetTriggerLevel(xWordFramesBuffer, KWS_FRAME_SHIFT_BYTES);

  for (;;) {
    size_t req_words = 0;
    xQueuePeek(xKWSRequestQueue, &req_words, portMAX_DELAY);

    if (req_words > MAX_WORDS) {
      ESP_LOGE(TAG, "req_words > MAX_WORDS");
    }
    ESP_LOGD(TAG, "recogninze req_words=%d", req_words);

    i2s_rx_slot_start();
    size_t det_words = 0;
    for (; det_words < req_words;) {
      WordDesc_t word = {.frame_num = 0, .max_abs = 0};
      while (xQueueReceive(xWordQueue, &word, pdMS_TO_TICKS(1)) == pdFAIL) {
        if (uxQueueMessagesWaiting(xKWSRequestQueue) == 0) {
          // canceled request
          xQueueReset(xKWSResultQueue);
          break;
        }
      }
      if (word.frame_num == 0) {
        goto CLEANUP;
      } else if (word.frame_num > 0) {
        ESP_LOGD(TAG, "got word: frame_num=%d, max_abs=%d", word.frame_num,
                 word.max_abs);
      }

      memset(proc_buf, 0, PROC_BUF_SZ);
      memset(mfcc_buffer, 0, MFCC_BUF_SZ);

      const int64_t t1 = esp_timer_get_time();
      const size_t mfcc_frames = std::min(
        (word.frame_num + MFCC_PROC_FRAME_NUM - 1) / MFCC_PROC_FRAME_NUM,
        size_t(KWS_FRAME_NUM));
      ESP_LOGD(TAG, "mfcc_frames=%d", mfcc_frames);

      auto xReceivedBytes = xStreamBufferReceive(xWordFramesBuffer, proc_buf,
                                                 KWS_FRAME_SHIFT_BYTES, 0);
      ESP_LOGV(TAG, "recv bytes=%d", xReceivedBytes);

      audio_t *half_proc_buf = (audio_t *)&proc_buf[KWS_FRAME_SHIFT_BYTES];

      size_t proc_frames = 0;
      for (; proc_frames < mfcc_frames;) {
        const auto xReceivedBytes = xStreamBufferReceive(
          xWordFramesBuffer, half_proc_buf, KWS_FRAME_SHIFT_BYTES, 0);
        ESP_LOGV(TAG, "recv bytes=%d", xReceivedBytes);

        preprocessor->MfccCompute((audio_t *)proc_buf,
                                  &mfcc_buffer[proc_frames * KWS_NUM_MFCC],
                                  word.max_abs);

        memmove(proc_buf, half_proc_buf, KWS_FRAME_SHIFT_BYTES);
        memset(half_proc_buf, 0, KWS_FRAME_SHIFT_BYTES);

        if (xReceivedBytes == 0) {
          break;
        } else {
          proc_frames++;
        }
      }

      memset(proc_buf, 0, PROC_BUF_SZ);
      for (size_t i = mfcc_frames; i < KWS_FRAME_NUM; i++) {
        memcpy(&mfcc_buffer[i * KWS_NUM_MFCC], silence_mfcc_coeffs,
               KWS_NUM_MFCC * sizeof(float));
      }
      ESP_LOGD(TAG, "preproc %d frames[%d]=%lld us", KWS_FRAME_NUM, det_words,
               esp_timer_get_time() - t1);

      if (word.frame_num > WORD_BUF_FRAME_NUM) {
        xStreamBufferReset(xWordFramesBuffer);
        ESP_LOGD(TAG, "cleared %d frames", word.frame_num - WORD_BUF_FRAME_NUM);
      }

      char result[32] = {0};
      int category = -1;
      if (nn_model_inference(model, mfcc_buffer, KWS_FEATURES_LEN, &category) <
          0) {
        ESP_LOGE(TAG, "inference error");
        continue;
      }
      nn_model_get_label(model, category, result, sizeof(result));
      ESP_LOGI(TAG, ">> kws: %s", result);
      xQueueSend(xKWSResultQueue, &category, 0);
      xSemaphoreGive(xKWSSema);
    }

  CLEANUP:
    xQueueReceive(xKWSRequestQueue, &req_words, 0);
    i2s_rx_slot_stop();
    xQueueReset(xWordQueue);
    xStreamBufferReset(xWordFramesBuffer);
    xEventGroupSetBits(xKWSEventGroup, KWS_STOP_MSK);
  }
}

int kws_task_init(kws_task_conf_t conf) {
  ESP_LOGD(TAG, "KWS_FRAME_LEN=%d, KWS_FRAME_SHIFT=%d, KWS_FRAME_NUM=%d",
           KWS_FRAME_LEN, KWS_FRAME_SHIFT, KWS_FRAME_NUM);
  ESP_LOGD(TAG, "KWS_FRAME_SZ=%d, KWS_FRAME_SHIFT_BYTES=%d", KWS_FRAME_SZ,
           KWS_FRAME_SHIFT_BYTES);
  ESP_LOGD(TAG, "PROC_BUF_FRAME_NUM=%d, WORD_BUF_FRAME_NUM=%d",
           PROC_BUF_FRAME_NUM, WORD_BUF_FRAME_NUM);

  s_agc_handle = esp_agc_open(3, CONFIG_MIC_SAMPLE_RATE);
  if (!s_agc_handle) {
    ESP_LOGE(TAG, "Unable to create agc");
    return -1;
  }
  set_agc_config(s_agc_handle, conf.mic_gain, 1, 0);

  s_ns_handle =
    ns_pro_create(AGC_FRAME_LEN_MS, conf.ns_level, CONFIG_MIC_SAMPLE_RATE);
  if (!s_ns_handle) {
    ESP_LOGE(TAG, "Unable to create esp_ns");
    return -1;
  }

  s_vad_handle = vad_create(VAD_MODE_4);
  if (!s_vad_handle) {
    ESP_LOGE(TAG, "Unable to create esp_vad");
    return -1;
  }

  xWordQueue = xQueueCreate(MAX_WORDS, sizeof(WordDesc_t));
  if (xWordQueue == NULL) {
    ESP_LOGE(TAG, "Error creating word queue");
    return -1;
  }
  xWordFramesBuffer = xStreamBufferCreate(WORD_BUF_SZ, MIC_FRAME_SZ);
  if (xWordFramesBuffer == NULL) {
    ESP_LOGE(TAG, "Error creating word stream buffer");
    return -1;
  }
  xKWSSema = xSemaphoreCreateBinary();
  if (!xKWSSema) {
    ESP_LOGE(TAG, "Error creating xKWSSema");
    return -1;
  }

  xKWSResultQueue = xQueueCreate(MAX_WORDS, sizeof(int));
  if (xKWSResultQueue == NULL) {
    ESP_LOGE(TAG, "Error creating KWS result queue");
    return -1;
  }
  xKWSRequestQueue = xQueueCreate(1, sizeof(size_t));
  if (xKWSRequestQueue == NULL) {
    ESP_LOGE(TAG, "Error creating KWS word queue");
    return -1;
  }
  xKWSEventGroup = xEventGroupCreate();
  if (xKWSEventGroup == NULL) {
    ESP_LOGE(TAG, "Error creating xKWSEventGroup");
    return -1;
  }

  auto xReturned =
    xTaskCreate(vad_task, "vad_task", configMINIMAL_STACK_SIZE + 1024 * 8, NULL,
                2, &xVADTaskHandle);
  if (xReturned != pdPASS) {
    ESP_LOGE(TAG, "Error creating vad_task");
    return -1;
  }

  s_kws_task_params.pp =
    new AudioPreprocessor(KWS_NUM_MFCC, KWS_FRAME_LEN, KWS_NUM_FBANK_BINS,
                          KWS_MEL_LOW_FREQ, KWS_MEL_HIGH_FREQ);
  s_kws_task_params.model_handle = conf.model_handle;
  xReturned =
    xTaskCreate(kws_task, "kws_task", configMINIMAL_STACK_SIZE + 1024 * 8,
                &s_kws_task_params, 1, &xKWSTaskHandle);
  if (xReturned != pdPASS) {
    ESP_LOGE(TAG, "Error creating kws_task");
    return -1;
  }

  xEventGroupSetBits(xKWSEventGroup, KWS_RUNNING_MSK);
  return 0;
}

void kws_task_release() {
  xEventGroupClearBits(xKWSEventGroup, KWS_RUNNING_MSK);
  kws_req_cancel();

  if (s_agc_handle) {
    esp_agc_close(s_agc_handle);
    s_agc_handle = NULL;
  }
  if (s_ns_handle) {
    ns_destroy(s_ns_handle);
    s_ns_handle = NULL;
  }
  if (s_vad_handle) {
    vad_destroy(s_vad_handle);
    s_vad_handle = NULL;
  }

  if (xWordQueue) {
    vQueueDelete(xWordQueue);
    xWordQueue = NULL;
  }
  if (xWordFramesBuffer) {
    vStreamBufferDelete(xWordFramesBuffer);
    xWordFramesBuffer = NULL;
  }
  if (xKWSSema) {
    vSemaphoreDelete(xKWSSema);
    xKWSSema = NULL;
  }
  if (xKWSRequestQueue) {
    vQueueDelete(xKWSRequestQueue);
    xKWSRequestQueue = NULL;
  }
  if (xKWSResultQueue) {
    vQueueDelete(xKWSResultQueue);
    xKWSResultQueue = NULL;
  }
  if (xKWSEventGroup) {
    vEventGroupDelete(xKWSEventGroup);
    xKWSEventGroup = NULL;
  }
  if (xVADTaskHandle) {
    vTaskDelete(xVADTaskHandle);
    xVADTaskHandle = NULL;
  }
  if (xKWSTaskHandle) {
    vTaskDelete(xKWSTaskHandle);
    xKWSTaskHandle = NULL;
  }
  if (s_kws_task_params.pp) {
    delete s_kws_task_params.pp;
    s_kws_task_params.pp = NULL;
  }
}

void kws_req_word(size_t req_words) {
  if (xEventGroupGetBits(xKWSEventGroup) & KWS_RUNNING_MSK) {
    if (xQueueSend(xKWSRequestQueue, &req_words, 0) == pdPASS) {
      xEventGroupClearBits(xKWSEventGroup, KWS_STOP_MSK);
    }
  }
}

void kws_req_cancel() {
  if (uxQueueMessagesWaiting(xKWSRequestQueue)) {
    xQueueReset(xKWSRequestQueue);
    xEventGroupWaitBits(xKWSEventGroup, KWS_STOP_MSK, pdTRUE, pdFALSE,
                        portMAX_DELAY);
  }
}
