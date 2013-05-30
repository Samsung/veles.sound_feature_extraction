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
#include <simd/arithmetic-inl.h>

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

#ifndef INPUT_TYPE
#define INPUT_TYPE float
#endif

#ifndef LENGTH
#define LENGTH 512
#endif

#ifndef OPT_STRING
#define OPT_STRING "SIMD"
#endif

#ifdef CLASS_NAME
TEST_F(CLASS_NAME, BENCH_NAME) {
#else
TEST(TEST_NAME, BENCH_NAME) {
#endif
#ifdef CUSTOM_CODE_PRE
  CUSTOM_CODE_PRE
#endif

  const int iter_count = ITER_COUNT;

  const int length = LENGTH;
  INPUT_TYPE *input = (INPUT_TYPE*)malloc_aligned(
      sizeof(INPUT_TYPE) * length);
#ifndef NO_OUTPUT
  INPUT_TYPE *output = (INPUT_TYPE*)malloc_aligned(
      sizeof(INPUT_TYPE) * length * 2);
#endif
  for (int i = 0; i < length; i++) {
    input[i] = sinf(i) * 10;
  }

  auto checkPointStart = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < iter_count; i++) {
#ifndef CUSTOM_FUNC_PEAK
    BENCH_FUNC(true, input, length
#ifndef PARAM_REORDER
#ifndef NO_OUTPUT
               , output
#endif
#endif
#ifdef EXTRA_PARAM
               , EXTRA_PARAM
#endif
#ifdef PARAM_REORDER
#ifndef NO_OUTPUT
               , output
#endif
#endif
               );
#else
    CUSTOM_FUNC_PEAK(input, length);
#endif
  }
  auto checkPointFinish = std::chrono::high_resolution_clock::now();
  auto delta1 = checkPointFinish - checkPointStart;
  checkPointStart = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < iter_count; i++) {
#ifndef CUSTOM_FUNC_BASELINE
    BENCH_FUNC(false, input, length
#ifndef PARAM_REORDER
#ifndef NO_OUTPUT
               , output
#endif
#endif
#ifdef EXTRA_PARAM
               , EXTRA_PARAM
#endif
#ifdef PARAM_REORDER
#ifndef NO_OUTPUT
               , output
#endif
#endif
               );
#else
    CUSTOM_FUNC_BASELINE(input, length);
#endif
  }
  checkPointFinish = std::chrono::high_resolution_clock::now();
  auto delta2 = checkPointFinish - checkPointStart;
  float ratio = (delta1.count() + .0f) / delta2.count();
  float speedup = (delta2.count() - delta1.count() + .0f) / delta2.count();
  printf(OPT_STRING " version took %i%% of original time. Speedup is %i%% "
         "(%.1f times).\n",
         static_cast<int>(ratio * 100), static_cast<int>(speedup * 100),
         1.f / (1.f - speedup));
  free(input);
#ifndef NO_OUTPUT
  free(output);
#endif
#ifdef CUSTOM_CODE_POST
  CUSTOM_CODE_POST
#endif
}

#endif
#endif
