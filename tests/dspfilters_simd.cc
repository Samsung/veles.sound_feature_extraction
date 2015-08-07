/*! @file dspfilters_simd.cc
 *  @brief Tests for SIMD implementation of the State class in DSPFilters.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright © 2013 Samsung R&D Institute Russia
 *
 *  @section License
 *  Licensed to the Apache Software Foundation (ASF) under one
 *  or more contributor license agreements.  See the NOTICE file
 *  distributed with this work for additional information
 *  regarding copyright ownership.  The ASF licenses this file
 *  to you under the Apache License, Version 2.0 (the
 *  "License"); you may not use this file except in compliance
 *  with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing,
 *  software distributed under the License is distributed on an
 *  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 *  KIND, either express or implied.  See the License for the
 *  specific language governing permissions and limitations
 *  under the License.
 */


#include <gtest/gtest.h>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wsequence-point"
#include <DspFilters/Dsp.h>
#pragma GCC diagnostic pop

TEST(DSPFilters, float_double) {
  Dsp::SimpleFilter<Dsp::ChebyshevII::BandPass<128>,
                    1, Dsp::DirectFormI<double, false>> base;
  base.setup(128, 22050, 5000, 200, 0.1);
  float input[] { 125, 100, -5, 1024 };
  float base_output[4];
  memcpy(base_output, input, sizeof(input));
  float *buffer = base_output;
  base.process(4, &buffer);
  Dsp::SimpleFilter<Dsp::ChebyshevII::BandPass<128>,
                    1, Dsp::DirectFormI<float, false>> peak;
  peak.setup(128, 22050, 5000, 200, 0.1);
  float peak_output[4];
  memcpy(peak_output, input, sizeof(input));
  buffer = peak_output;
  peak.process(4, &buffer);
  for (int i = 0; i < 4; i++) {
    ASSERT_NEAR(base_output[i], peak_output[i], fabs(base_output[i] / 1000))
        << i;
  }
}

TEST(DSPFilters, float_simd) {
  Dsp::SimpleFilter<Dsp::ChebyshevII::BandPass<128>,
                    1, Dsp::DirectFormI<float, false>> base;
  base.setup(128, 22050, 5000, 200, 0.1);
  float input[] { 125, 100, -5, 1024 };
  float base_output[4];
  memcpy(base_output, input, sizeof(input));
  float *buffer = base_output;
  base.process(4, &buffer);
  Dsp::SimpleFilter<Dsp::ChebyshevII::BandPass<128>,
                    1, Dsp::DirectFormI<float, true>> peak;
  peak.setup(128, 22050, 5000, 200, 0.1);
  float peak_output[4];
  memcpy(peak_output, input, sizeof(input));
  buffer = peak_output;
  peak.process(4, &buffer);
  for (int i = 0; i < 4; i++) {
    ASSERT_NEAR(base_output[i], peak_output[i], fabs(base_output[i] / 1000))
        << i;
  }
}

TEST(DSPFilters, float_benchmark) {
  Dsp::SimpleFilter<Dsp::ChebyshevII::BandPass<128>,
                    1, Dsp::DirectFormI<float, false>> base;
  base.setup(128, 22050, 5000, 200, 0.1);
  float input[] { 125, 100, -5, 1024 };
  float base_output[4];
  memcpy(base_output, input, sizeof(input));
  float *buffer = base_output;
  for (int i = 0; i < 500000; i++) {
    base.process(4, &buffer);
  }
}

TEST(DSPFilters, simd_benchmark) {
  Dsp::SimpleFilter<Dsp::ChebyshevII::BandPass<128>,
                    1, Dsp::DirectFormI<float, true>> peak;
  peak.setup(128, 22050, 5000, 200, 0.1);
  float input[] { 125, 100, -5, 1024 };
  float peak_output[4];
  memcpy(peak_output, input, sizeof(input));
  float *buffer = peak_output;
  for (int i = 0; i < 500000; i++) {
    peak.process(4, &buffer);
  }
}

#include "tests/google/src/gtest_main.cc"
