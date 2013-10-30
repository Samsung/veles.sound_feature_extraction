/*! @file all_features.cc
 *  @brief Get all the available features..
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include <gtest/gtest.h>
#include <fftf/api.h>
#include "src/transform_tree.h"
#include "src/transform_registry.h"
#include "src/formats/array_format.h"
#include "tests/speech_sample.inc"

using SoundFeatureExtraction::TransformTree;
using SoundFeatureExtraction::BuffersBase;

TEST(Features, All) {
  fftf_available_backends(nullptr, nullptr);
  TransformTree tt( { 48000, 16000 } );  // NOLINT(*)
  tt.AddFeature("Energy", { { "Window", "type=rectangular" }, { "Window", "" },
      { "Energy", "" }
  });
  tt.AddFeature("Centroid", { { "Window", "type=rectangular" }, { "Window", "" },
      { "RDFT", "" }, { "ComplexMagnitude", "" }, { "Centroid", "" }
  });
  tt.AddFeature("Rolloff", { { "Window", "type=rectangular" }, { "Window", "" },
      { "RDFT", "" }, { "ComplexMagnitude", "" }, { "Rolloff", "" }
  });
  tt.AddFeature("Flux", { { "Window", "type=rectangular" }, { "Window", "" },
      { "RDFT", "" }, { "ComplexMagnitude", "" }, { "Flux", "" }
  });
  tt.AddFeature("ZeroCrossings", {
      { "Window", "type=rectangular,length=512,step=205" },
      { "ZeroCrossings", "" },
      { "Merge", "" },
      { "Stats", "interval=50" }
  });
  tt.AddFeature("WPP", { { "Preemphasis", "value=0.2" },
      { "Window", "type=rectangular" },
      { "DWPT", "" }, { "SubbandEnergy", "" }, { "Log", "" },
      /*{ "Square", "" },*/ { "DWPT", "order=4, tree=1 2 3 3" },
      { "Selector", "length=16, threads_num=1" }, { "STMSN", "length=25" }
  });
  tt.AddFeature("WPP_D1", { { "Preemphasis", "value=0.2" },
      { "Window", "type=rectangular" },
      { "DWPT", "" }, { "SubbandEnergy", "" }, { "Log", "" },
      /*{ "Square", "" },*/ { "DWPT", "order=4, tree=1 2 3 3" },
      { "Selector", "length=16, threads_num=1" }, { "Delta", "" },
      { "STMSN", "length=25" }
  });
  tt.AddFeature("WPP_D2", { { "Preemphasis", "value=0.2" },
      { "Window", "type=rectangular" },
      { "DWPT", "" }, { "SubbandEnergy", "" }, { "Log", "" },
      /*{ "Square", "" },*/ { "DWPT", "order=4, tree=1 2 3 3" },
      { "Selector", "length=16, threads_num=1" }, { "Delta", "" },
      { "Delta", "" }, { "STMSN", "length=25" }
  });
  tt.AddFeature("SFM", { { "Window", "type=rectangular" }, { "Window", "" },
      { "RDFT", "" }, { "ComplexMagnitude", "" },
      { "Mean", "types=arithmetic geometric" }, { "SFM", "" }
  });
  tt.AddFeature("DominantFrequency", { { "Window", "type=rectangular" },
      { "Window", "" }, { "RDFT", "" },
      { "ComplexMagnitude", "" }, { "ArgMinMax", "extremum=max" }
  });
  tt.AddFeature("SBC", { { "Preemphasis", "value=0.2" },
      { "Window", "type=rectangular" },
      { "DWPT", "" }, { "SubbandEnergy", "" }, { "Log", "" },
      /*{ "Square", "" },*/ { "ZeroPadding", "" }, { "DCT", "" },
      { "Selector", "length=16, threads_num=1" }, { "STMSN", "length=25" }
  });
  tt.AddFeature("SBC_D1", { { "Preemphasis", "value=0.2" },
      { "Window", "type=rectangular" },
      { "DWPT", "" }, { "SubbandEnergy", "" }, { "Log", "" },
      /*{ "Square", "" },*/ { "ZeroPadding", "" }, { "DCT", "" },
      { "Selector", "length=16, threads_num=1" }, { "Delta", "" },
      { "STMSN", "length=25" }
  });
  tt.AddFeature("SBC_D2", { { "Preemphasis", "value=0.2" },
      { "Window", "type=rectangular" },
      { "DWPT", "" }, { "SubbandEnergy", "" }, { "Log", "" },
      /*{ "Square", "" },*/ { "ZeroPadding", "" }, { "DCT", "" },
      { "Selector", "length=16, threads_num=1" }, { "Delta", "" },
      { "Delta", "" },{ "STMSN", "length=25" }
  });
  tt.AddFeature("MFCC", { { "Preemphasis", "value=0.2" },
      { "Window", "type=rectangular" }, { "Window", "" },
      { "RDFT", "" }, { "SpectralEnergy", "" }, { "FilterBank", "" },
      { "FilterBank", "" }, { "Log", "" }, { "Square", "" },
      { "Selector", "length=256, threads_num=1" }, { "DCT", "" },
      { "Selector", "length=16, threads_num=1" }, { "STMSN", "length=25" }
  });
  tt.AddFeature("MFCC_D1", { { "Preemphasis", "value=0.2" },
      { "Window", "type=rectangular" }, { "Window", "" },
      { "RDFT", "" }, { "SpectralEnergy", "" }, { "FilterBank", "" },
      { "FilterBank", "" }, { "Log", "" }, { "Square", "" },
      { "Selector", "length=256, threads_num=1" }, { "DCT", "" },
      { "Selector", "length=16, threads_num=1"}, { "Delta", "" },
      { "STMSN", "length=25" }
  });
  tt.AddFeature("MFCC_D2", { { "Preemphasis", "value=0.2" },
      { "Window", "type=rectangular" }, { "Window", "" },
      { "RDFT", "" }, { "SpectralEnergy", "" }, { "FilterBank", "" },
      { "FilterBank", "" }, { "Log", "" }, { "Square", "" },
      { "Selector", "length=256, threads_num=1" }, { "DCT", "" },
      { "Selector", "length=16, threads_num=1"}, { "Delta", "" },
      { "Delta", "" }, { "STMSN", "length=25" }
  });
  tt.AddFeature("F0_HPS", { { "Window", "type=rectangular" }, { "Window", "" },
      { "RDFT", "" }, { "ComplexMagnitude", "" }, { "SHC", "" }, { "Peaks", "" }
  });
  int16_t* buffers = new int16_t[48000];
  memcpy(buffers, data, sizeof(data));
  tt.PrepareForExecution();
  auto res = tt.Execute(buffers);
  delete[] buffers;
  ASSERT_EQ(17U, res.size());
  printf("Validating Energy\n");
  res["Energy"]->Validate();
  printf("Validating Centroid\n");
  res["Centroid"]->Validate();
  printf("Validating Rolloff\n");
  res["Rolloff"]->Validate();
  printf("Validating Flux\n");
  res["Flux"]->Validate();
  printf("Validating ZeroCrossings\n");
  res["ZeroCrossings"]->Validate();
  printf("Validating WPP\n");
  res["WPP"]->Validate();
  printf("Validating WPP_D1\n");
  res["WPP_D1"]->Validate();
  printf("Validating WPP_D2\n");
  res["WPP_D2"]->Validate();
  printf("Validating SBC\n");
  res["SBC"]->Validate();
  printf("Validating SBC_D1\n");
  res["SBC_D1"]->Validate();
  printf("Validating SBC_D2\n");
  res["SBC_D2"]->Validate();
  printf("Validating DominantFrequency\n");
  res["DominantFrequency"]->Validate();
  printf("Validating MFCC\n");
  res["MFCC"]->Validate();
  printf("Validating MFCC_D1\n");
  res["MFCC_D1"]->Validate();
  printf("Validating MFCC_D2\n");
  res["MFCC_D2"]->Validate();
  printf("Validating F0_HPS\n");
  res["F0_HPS"]->Validate();
  tt.Dump("/tmp/all.dot");
  auto report = tt.ExecutionTimeReport();
  for (auto r : report) {
    printf("%s:\t%f\n", r.first.c_str(), r.second);
  }
}

#include "tests/google/src/gtest_main.cc"
