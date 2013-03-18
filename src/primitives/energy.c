/*! @file energy.c
 *  @brief Low level energy calculation.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/primitives/energy.h"
#ifdef __AVX__
#include <immintrin.h>
#elif defined(__ARM_NEON__)
#include <arm_neon.h>
#endif

float CalculateEnergy(const float *signal, size_t length) {
  float energy = .0f;
#ifdef __AVX__
  __m256 accum = { .0f };
  for (int j = 0; j < (int)length - 7; j += 8) {
    __m256 vec = _mm256_loadu_ps(signal + j);
    vec = _mm256_dp_ps(vec, vec, 0xFF);
    accum = _mm256_add_ps(accum, vec);
  }
  energy += accum[0] + accum[4];
  for (int j = ((length >> 3) << 3); j < (int)length; j++) {
    float val = signal[j];
    energy += val * val;
  }
#elif defined(__ARM_NEON__)
    float32x4_t accum = { .0f };
    for (int j = 0; j < length - 3; j += 4) {
      float32x4_t vec = vld1q_f32(signal + j);
      accum = vmlaq_f32(accum, vec, vec);
    }
    float32x2_t sums = vpadd_f32(vget_high_f32(accum),
                                 vget_low_f32(accum));
    energy += vget_lane_f32(sums, 0) + vget_lane_f32(sums, 1);
    for (int j = ((length >> 2) << 2); j < (int)length; j++) {
      float val = signal[j];
      energy += val * val;
    }
#else
    for (int j = 0; j < length; j++) {
      float val = signal[j];
      energy += val * val;
    }
#endif
  return energy / length;
}
