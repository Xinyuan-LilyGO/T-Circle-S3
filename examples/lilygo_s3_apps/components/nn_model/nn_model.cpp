#include "esp_log.h"
#include "esp_timer.h"

#include "string.h"

#include "nn_model.h"
#include "tensor_arena.h"
#include "tflite_op_resolver.h"

#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"

struct __nn_model_t {
  tflite::MicroInterpreter *interpreter;
  nn_model_config_t cfg;
};

typedef __nn_model_t *__nn_model_handle_t;

static void set_input(const float *src, TfLiteTensor *tensor, size_t len,
                      bool is_qnn) {
  if (is_qnn) {
    for (int i = 0; i < len; i++) {
      tensor->data.int8[i] =
        (int8_t)(src[i] / tensor->params.scale + tensor->params.zero_point);
    }
  } else {
    for (int i = 0; i < len; i++) {
      tflite::GetTensorData<float>(tensor)[i] = src[i];
    }
  }
}

static void get_output(const TfLiteTensor *tensor, float *dst, size_t len,
                       bool is_qnn) {
  if (is_qnn) {
    for (int i = 0; i < len; i++) {
      dst[i] = (tensor->data.int8[i] - tensor->params.zero_point) *
               tensor->params.scale;
    }
  } else {
    for (int i = 0; i < len; i++) {
      dst[i] = tflite::GetTensorData<float>(tensor)[i];
    }
  }
}

static size_t argmax(float *array, size_t len) {
  size_t idx = 0;
  for (size_t i = 0; i < len; i++) {
    if (array[i] > array[idx])
      idx = i;
  }
  return idx;
}

int nn_model_init(nn_model_handle_t *model_handle, nn_model_config_t cfg) {
  __nn_model_handle_t __nn_model_handle =
    static_cast<__nn_model_handle_t>(malloc(sizeof(__nn_model_t)));
  if (!__nn_model_handle) {
    ESP_LOGW(__FUNCTION__, "unable to allocate model_handle");
    return -1;
  }

  const tflite::Model *model = tflite::GetModel(cfg.model_ptr);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    ESP_LOGE(
      __FUNCTION__,
      "Model provided is schema version %u not equal to supported version %d",
      model->version(), TFLITE_SCHEMA_VERSION);
    free(__nn_model_handle);
    return -1;
  }

  uint8_t *tensor_arena = TensorArena::getBuffer();
  if (!tensor_arena) {
    ESP_LOGE(__FUNCTION__, "unable to get tensor arena");
    free(__nn_model_handle);
    return -1;
  }
  // Build an interpreter to run the model with.
  __nn_model_handle->interpreter =
    new tflite::MicroInterpreter(model, TFLiteOpResolver::getInstance(),
                                 tensor_arena, TensorArena::getSize());

  // Allocate memory from the tensor_arena for the model's tensors.
  TfLiteStatus allocate_status =
    __nn_model_handle->interpreter->AllocateTensors();
  if (allocate_status != kTfLiteOk) {
    ESP_LOGE(__FUNCTION__, "AllocateTensors() failed");
    free(__nn_model_handle);
    return -1;
  }

  *model_handle = __nn_model_handle;
  memcpy(&__nn_model_handle->cfg, &cfg, sizeof(nn_model_config_t));
  return 0;
}

int nn_model_release(nn_model_handle_t model_handle) {
  if (model_handle) {
    TensorArena::releaseBuffer();
    __nn_model_handle_t __nn_model_handle =
      static_cast<__nn_model_handle_t>(model_handle);
    delete __nn_model_handle->interpreter;
    free(__nn_model_handle);
  }
  return 0;
}

int nn_model_get_label(nn_model_handle_t model_handle, int category,
                       char *buffer, size_t len) {
  if (!model_handle) {
    ESP_LOGE(__FUNCTION__, "nn model is not initialized");
    return -1;
  }
  __nn_model_handle_t __nn_model_handle =
    static_cast<__nn_model_handle_t>(model_handle);
  nn_model_config_t &cfg = __nn_model_handle->cfg;
  if (cfg.labels) {
    if (category < 0) {
      strncpy(buffer, cfg.labels[1], len);
    } else if (category < cfg.labels_num) {
      strncpy(buffer, cfg.labels[category], len);
    }
  }
  return 0;
}

int nn_model_inference(nn_model_handle_t model_handle, const float *input_data,
                       size_t len, int *category) {
  if (!model_handle) {
    ESP_LOGE(__FUNCTION__, "nn model is not initialized");
    return -1;
  }
  __nn_model_handle_t __nn_model_handle =
    static_cast<__nn_model_handle_t>(model_handle);
  nn_model_config_t &cfg = __nn_model_handle->cfg;

  const int64_t t1 = esp_timer_get_time();
  set_input(input_data, __nn_model_handle->interpreter->input(0), len,
            cfg.is_quantized);

  TfLiteStatus invoke_status = __nn_model_handle->interpreter->Invoke();
  if (invoke_status != kTfLiteOk) {
    ESP_LOGE(__FUNCTION__, "Invoke failed");
    return -1;
  }

  float *out_buffer = new float[cfg.labels_num];
  if (!out_buffer) {
    ESP_LOGE(__FUNCTION__, "unable to allocate out buffer");
    return -1;
  }
  get_output(__nn_model_handle->interpreter->output(0), out_buffer,
             cfg.labels_num, cfg.is_quantized);

  const size_t idx = argmax(out_buffer, cfg.labels_num);
  char result[32];
  nn_model_get_label(model_handle, idx, result, sizeof(result));

  ESP_LOGI(__FUNCTION__, "%f, %s, %lld", out_buffer[idx], result,
           esp_timer_get_time() - t1);
  *category = -1;
  if (out_buffer[idx] > cfg.inference_threshold) {
    *category = idx;
  }
  delete[] out_buffer;
  return 0;
}
