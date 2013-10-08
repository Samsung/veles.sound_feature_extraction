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
#include <simd/memory.h>

float calculate_energy(int simd, const float *signal, size_t length) {
  float energy = 0.f;
  int ilength = (int)length;
  if (simd) {
#ifdef __AVX__
    __m256 accum = { 0.f };
    int startIndex = align_complement_f32(signal);
    for (int j = 0; j < startIndex; j++) {
      float val = signal[j];
      energy += val * val;
    }

    for (int j = startIndex; j < ilength - 7; j += 8) {
      __m256 vec = _mm256_load_ps(signal + j);
      vec = _mm256_mul_ps(vec, vec);
      accum = _mm256_add_ps(accum, vec);
    }
    accum = _mm256_hadd_ps(accum, accum);
    accum = _mm256_hadd_ps(accum, accum);
    energy += accum[0] + accum[4];

    for (int j = startIndex + (((ilength - startIndex) >> 3) << 3);
        j < ilength; j++) {
      float val = signal[j];
      energy += val * val;
    }
  } else {
#elif defined(__ARM_NEON__)
    float32x4_t accum = { 0.f };
    for (int j = 0; j < ilength - 3; j += 4) {
      float32x4_t vec = vld1q_f32(signal + j);
      accum = vmlaq_f32(accum, vec, vec);
    }
    float32x2_t sums = vpadd_f32(vget_high_f32(accum),
                                 vget_low_f32(accum));
    energy += vget_lane_f32(sums, 0) + vget_lane_f32(sums, 1);
    for (int j = ((ilength >> 2) << 2); j < ilength; j++) {
      float val = signal[j];
      energy += val * val;
    }
  } else {
#else
  } {
#endif
    for (int j = 0; j < ilength; j++) {
      float val = signal[j];
      energy += val * val;
    }
  }
  return energy / length;
}
