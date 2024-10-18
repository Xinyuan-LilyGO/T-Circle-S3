#ifndef _TFLITE_OP_RESOLVER_H_
#define _TFLITE_OP_RESOLVER_H_

#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"

class TFLiteOpResolver {
public:
  static const tflite::MicroOpResolver &getInstance() {
    static TFLiteOpResolver instance;
    return instance.op_resolver_;
  }
  TFLiteOpResolver(TFLiteOpResolver const &) = delete;
  void operator=(TFLiteOpResolver const &) = delete;

private:
  tflite::MicroMutableOpResolver<7> op_resolver_;
  TFLiteOpResolver() {
    op_resolver_.AddAveragePool2D();
    op_resolver_.AddConv2D();
    op_resolver_.AddDepthwiseConv2D();
    op_resolver_.AddFullyConnected();
    op_resolver_.AddRelu();
    op_resolver_.AddSoftmax();
    op_resolver_.AddReshape();
  }
};

#endif // _TFLITE_OP_RESOLVER_H_