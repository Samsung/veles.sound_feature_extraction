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
#include "src/formats/raw_format.h"
#include "tests/speech_sample.inc"

using SoundFeatureExtraction::TransformTree;
using SoundFeatureExtraction::Formats::Raw16;
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
  tt.AddFeature("ZeroCrossings", { { "ZeroCrossings", "" }
  });
  tt.AddFeature("WPP", { { "Window", "type=rectangular" },
      { "DWPT", "" }, { "SubbandEnergy", "" }, { "Log", "" },
      /*{ "Square", "" },*/ { "DWPT", "order=4, tree=1 2 3 3" },
      { "Selector", "length=16" }
  });
  tt.AddFeature("SFM", { { "Window", "type=rectangular" }, { "Window", "" },
      { "RDFT", "" }, { "ComplexMagnitude", "" },
      { "Mean", "types=arithmetic geometric" }, { "SFM", "" }
  });
  tt.AddFeature("DominantFrequency", { { "Window", "type=rectangular" },
      { "Window", "" }, { "RDFT", "" },
      { "ComplexMagnitude", "" }, { "ArgMinMax", "extremum=max" }
  });
  tt.AddFeature("SBC", { { "Window", "type=rectangular" },
      { "DWPT", "" }, { "SubbandEnergy", "" }, { "Log", "" },
      /*{ "Square", "" },*/ { "ZeroPadding", "" }, { "DCT", "" },
      { "Selector", "length=16" }
  });
  tt.AddFeature("MFCC", { { "Window", "type=rectangular" }, { "Window", "" },
      { "RDFT", "" }, { "SpectralEnergy", "" }, { "FilterBank", "" },
      { "FilterBank", "" }, { "Log", "" }, { "Square", "" },
      { "Selector", "length=256" }, { "DCT", "" },
      { "Selector", "length=16" }
  });
  tt.AddFeature("F0_HPS", { { "Window", "type=rectangular" }, { "Window", "" },
      { "RDFT", "" }, { "ComplexMagnitude", "" }, { "HPS", "" }
  });
  Raw16 buffers(48000, 0);
  memcpy(buffers.Data.get(), data, sizeof(data));
  tt.PrepareForExecution();
  auto res = tt.Execute(buffers);
  ASSERT_EQ(11, res.size());
  res["Energy"]->Validate();
  res["Centroid"]->Validate();
  res["Rolloff"]->Validate();
  res["Flux"]->Validate();
  res["ZeroCrossings"]->Validate();
  res["WPP"]->Validate();
  res["SBC"]->Validate();
  res["Energy"]->Validate();
  res["DominantFrequency"]->Validate();
  res["MFCC"]->Validate();
  res["F0_HPS"]->Validate();
  tt.Dump("/tmp/all.dot");
  auto report = tt.ExecutionTimeReport();
  for (auto r : report) {
    printf("%s:\t%f\n", r.first.c_str(), r.second);
  }
}

#include "tests/google/src/gtest_main.cc"
