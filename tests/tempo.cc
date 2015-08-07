/*! @file sbc.cc
 *  @brief Subband based Cepstral Parameters.
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
#include <fstream>
#include "src/transform_tree.h"
#include "src/transform_registry.h"
#include "src/formats/array_format.h"
#include "tests/speech_sample.inc"

using sound_feature_extraction::TransformTree;
using sound_feature_extraction::BuffersBase;

TEST(Features, Tempo) {
  size_t test_size;
 /* std::string file_name = "/home/markhor/Development/SoundFeatureExtraction/fail.raw";
  std::ifstream input(file_name);
  input.seekg(0, std::ios::end);
  test_size = input.tellg() / 2;
  input.seekg(0, std::ios::beg);
  std::vector<char> fdata(test_size * 2);
  input.read(&fdata[0], test_size * 2);
*/
  test_size = 100000;  // 661504;
  TransformTree tt( { test_size, 22050 } );  // NOLINT(*)
  tt.set_validate_after_each_transform(true);
  tt.AddFeature("Tempo", {
      { "Window", "type=rectangular,length=512,step=205" },
      { "Window", "type=hamming" },
      { "Fork", "factor=6" },
      { "RDFT", "" },
      { "FrequencyBands", "bands=200 400 800 1600 3200" },
      { "IRDFT", "" },
      { "IWindow", "length=512,count=6,interleaved=true" },
      { "Rectify", "" },
      { "Convolve", "window=half-hanning-right, length=12800" },
      { "Diff", "rectify=true" },
      { "Beat", "bands=6" } });
  int16_t* buffers = new int16_t[test_size];
  size_t i;
  for (i = 0; i < test_size - sizeof(data) / 2 + 1;
      i += sizeof(data) / 2) {
    memcpy(buffers + i, data, sizeof(data));
  }
  memcpy(buffers + i, data, (test_size - i) * 2);
  // memcpy(buffers, &fdata[0], test_size * 2);
  tt.PrepareForExecution();
  auto res = tt.Execute(buffers);
  delete[] buffers;
  ASSERT_EQ(1U, res.size());
  res["Tempo"]->Validate();
  tt.Dump("/tmp/tempo.dot");
  auto report = tt.ExecutionTimeReport();
  for (auto r : report) {
    printf("%s:\t%f\n", r.first.c_str(), r.second);
  }
}

#include "tests/google/src/gtest_main.cc"
