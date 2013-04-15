/*! @file benchmark.inc
 *  @brief Simple transform benchmark code generator.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include <math.h>
#include <chrono>
#include "src/primitives/arithmetic-inl.h"

#ifdef BENCHMARK

// Pointless to benchmark code without SIMD acceleration
#ifdef SIMD

#ifndef CLASS_NAME
#ifndef TEST_NAME
#error Must define CLASS_NAME or TEST_NAME before including this file.
#endif
#endif

#ifndef ITER_COUNT
#define ITER_COUNT 100000
#endif

#ifndef BENCH_FUNC
#define BENCH_FUNC Do
#endif

#ifndef BENCH_NAME
#define BENCH_NAME Benchmark
#endif

#ifdef CLASS_NAME
TEST_F(CLASS_NAME, BENCH_NAME) {
#else
TEST(TEST_NAME, BENCH_NAME) {
#endif
  const int iter_count = ITER_COUNT;

  const int length = 512;
  float input[length] __attribute__ ((aligned (64)));
#ifndef NO_OUTPUT
  float output[length * 2] __attribute__ ((aligned (64)));
#endif
  for (int i = 0; i < length; i++) {
    input[i] = sinf(i);
  }

  auto checkPointStart = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < iter_count; i++) {
    BENCH_FUNC(true, input, length
#ifndef NO_OUTPUT
               , output
#endif
#ifdef EXTRA_PARAM
               , EXTRA_PARAM
#endif
               );
  }
  auto checkPointFinish = std::chrono::high_resolution_clock::now();
  auto delta1 = checkPointFinish - checkPointStart;
  checkPointStart = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < iter_count; i++) {
    BENCH_FUNC(false, input, length
#ifndef NO_OUTPUT
               , output
#endif
#ifdef EXTRA_PARAM
               , EXTRA_PARAM
#endif
               );
  }
  checkPointFinish = std::chrono::high_resolution_clock::now();
  auto delta2 = checkPointFinish - checkPointStart;
  float ratio = (delta1.count() + .0f) / delta2.count();
  float speedup = (delta2.count() - delta1.count() + .0f) / delta2.count();
  printf("SIMD version took %i%% of original time. Speedup is %i%%.\n",
      static_cast<int>(ratio * 100), static_cast<int>(speedup * 100));
}

#endif
#endif
