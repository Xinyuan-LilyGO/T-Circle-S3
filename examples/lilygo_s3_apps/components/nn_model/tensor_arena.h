#ifndef _TENSOR_ARENA_H_
#define _TENSOR_ARENA_H_

#include "esp_log.h"

class TensorArena {
public:
  static uint8_t *getBuffer() {
    TensorArena &instance = getInstance();
    if (!instance.used_) {
      instance.used_ = true;
      return instance.buffer_;
    } else {
      ESP_LOGW(__FUNCTION__, "Tensor arena buffer is used");
      return nullptr;
    }
  }
  static void releaseBuffer() { getInstance().used_ = false; }
  static size_t getSize() { return kTensorArenaSize_; }
  TensorArena(TensorArena const &) = delete;
  void operator=(TensorArena const &) = delete;

private:
  static TensorArena &getInstance() {
    static TensorArena instance;
    return instance;
  }
  TensorArena() : used_(false) {
    buffer_ = new uint8_t[kTensorArenaSize_];
    if (!buffer_) {
      ESP_LOGE(__FUNCTION__, "Unable to allocate tensor arena buffer");
    }
  }
  bool used_;
  uint8_t *buffer_;
  static constexpr size_t kTensorArenaSize_ = 108 * 1024;
};

#endif // _TENSOR_ARENA_H_