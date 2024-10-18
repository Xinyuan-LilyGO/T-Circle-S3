#ifndef _MIC_PROC_H_
#define _MIC_PROC_H_

#include "stddef.h"
#include "stdint.h"
#include "string.h"

/*! \brief Biquad direct form 1, order 1 filter. */
struct biquad_df1_o1_filter {
private:
  int32_t bcoeffs_[3];
  int32_t acoeffs_[3];
  int32_t xn_[2];
  int32_t yn_[2];
  size_t shift_;

public:
  biquad_df1_o1_filter(const int32_t *bcoeffs, const int32_t *acoeffs,
                       size_t shift)
    : shift_(shift) {
    memcpy(bcoeffs_, bcoeffs, sizeof(bcoeffs_));
    memcpy(acoeffs_, acoeffs, sizeof(acoeffs_));
    reset();
  }

  void proc_buffer(int32_t *dst, const int32_t *src, size_t len) {
    for (size_t i = 0; i < len; i++) {
      dst[i] = proc_val(src[i]);
    }
  }
  void proc_buffer(int16_t *dst, const int16_t *src, size_t len) {
    for (size_t i = 0; i < len; i++) {
      dst[i] = proc_val(src[i]);
    }
  }
  int32_t proc_val(int32_t val) {
    // acc =  b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1] + a2 * y[n-2]
    int64_t acc = int64_t(bcoeffs_[0]) * val;

    // acc +=  b1 * x[n-1]
    acc += int64_t(bcoeffs_[1]) * xn_[0];
    // acc +=  b[2] * x[n-2]
    acc += int64_t(bcoeffs_[2]) * xn_[1];
    // acc +=  a1 * y[n-1]
    acc += int64_t(acoeffs_[1]) * yn_[0];
    // acc +=  a2 * y[n-2]
    acc += int64_t(acoeffs_[2]) * yn_[1];

    acc = acc >> shift_;

    // x[n-2] = x[n-1]
    // x[n-1] = Xn
    // y[n-2] = y[n-1]
    // y[n-1] = acc
    xn_[1] = xn_[0];
    xn_[0] = val;
    yn_[1] = yn_[0];
    yn_[0] = acc;

    return acc;
  }
  void reset() {
    memset(xn_, 0, sizeof(xn_));
    memset(yn_, 0, sizeof(yn_));
  }
};

template <typename T> struct dc_blocker {
private:
  T xM_ = 0;
  T yM_ = 0;
  T y_ = 0;

public:
  T proc_val(T val) {
    y_ = val - xM_ + 0.995 * yM_;
    xM_ = val;
    yM_ = y_;
    return y_;
  }
};

#endif // _MIC_PROC_H_
