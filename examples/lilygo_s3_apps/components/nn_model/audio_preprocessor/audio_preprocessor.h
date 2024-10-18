/*
 * Copyright (C) 2020 Arm Limited or its affiliates. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __KWS_MFCC_H__
#define __KWS_MFCC_H__

extern "C" {
#include <math.h>
#include <string.h>
}

#include "dsp/fast_math_functions.h"
#include "dsp/transform_functions.h"
#include <vector>

#define SAMP_FREQ CONFIG_MIC_SAMPLE_RATE

#define M_2PI 6.283185307179586476925286766559005
#ifndef M_PI    /* M_PI might not be defined for non-gcc based tc */
#define M_PI PI /* Comes from math.h */
#endif          /* M_PI */

class AudioPreprocessor {
private:
  int numMfccFeatures;
  int frameLen;
  int frameLenPadded;
  int numFbankBins;
  std::vector<float> frame;
  std::vector<float> buffer;
  std::vector<float> melEnergies;
  std::vector<float> windowFunc;
  std::vector<int32_t> fbankFilterFirst;
  std::vector<int32_t> fbankFilterLast;
  std::vector<std::vector<float>> melFbank;
  std::vector<float> dctMatrix;
  riscv_rfft_fast_instance_f32 fft;
  static std::vector<float> CreateDctMatrix(int32_t inputLength,
                                            int32_t coefficientCount);
  std::vector<std::vector<float>> CreateMelFbank(int melLowF, int melHighF);

  static inline float InverseMelScale(float melFreq) {
    return 700.0f * (expf(melFreq / 1127.0f) - 1.0f);
  }

  static inline float MelScale(float freq) {
    return 1127.0f * logf(1.0f + freq / 700.0f);
  }

public:
  AudioPreprocessor(int numMfccFeatures, int frameLen, int numFbankBins,
                    int melLowF, int melHighF);
  ~AudioPreprocessor() = default;

  void MfccCompute(const int16_t *data, float *mfccOut, size_t max_abs);
  void LogMelCompute(const int16_t *data, float *mfccOut, size_t max_abs);
};

#endif
