/*! @file beat_dynamic_programming.cc
 *  @brief Peaks analysis.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/beat_dynamic_programming.h"
#include <cmath>
#include <vector>

namespace SoundFeatureExtraction {
namespace Transforms {

void BeatDynamicProgramming::Do(const BuffersBase<Formats::FixedArray<2>*>& in,
                                BuffersBase<float>* out)
    const noexcept {
  std::vector<std::vector<std::tuple<float, int>>> costs(in.Count());
  std::vector<int> candidates_counts(in.Count());
  for (size_t i = 0; i < in.Count(); i++) {
    int candidates_count = 0;
    for (size_t j = 0; in[i][j][0] != 0 && j < inputFormat_->Size(); j++) {
      candidates_count++;
    }
    candidates_counts[i] = candidates_count;
    costs[i].resize(candidates_count);
    if (i > 0) {
      for (int j = 0; j < candidates_count; j++) {
        float min_cost = std::numeric_limits<float>::max();
        int index = -1;
        for (int k = 0; k < candidates_counts[i - 1]; k++) {
          float cost = fabsf(in[i][j][0] - in[i - 1][k][0]) *
              in[i - 1][k][1] / in[i][j][1] + std::get<0>(costs[i - 1][k]);
          if (cost < min_cost) {
            min_cost = cost;
            index = k;
          }
        }
        costs[i][j] = std::make_tuple(min_cost, index);
      }
    } else {
      for (int j = 0; j < candidates_count; j++) {
        costs[0][j] = std::make_tuple(0, j);
      }
    }
  }
  for (int j = 0; j < candidates_counts[0]; j++) {
    float min_cost = std::numeric_limits<float>::max();
    int index = -1;
    for (int k = 0; k < candidates_counts[1]; k++) {
      float cost = fabsf(in[0][j][0] - in[1][k][0]) *
          in[1][k][1] / in[0][j][1];
      if (cost < min_cost) {
        min_cost = cost;
        index = k;
      }
    }
    costs[0][j] = std::make_tuple(min_cost, index);
  }
  for (int i = in.Count() - 1; i >= 0; i--) {
    float min_cost = std::numeric_limits<float>::max();
    int index = -1;
    for (int j = 0; j < candidates_counts[i]; j++) {
      float cost = std::get<0>(costs[i][j]);
      if (cost < min_cost) {
        min_cost = cost;
        index = j;
      }
    }
    (*out)[i] = in[i][index][0];
  }
}

REGISTER_TRANSFORM(BeatDynamicProgramming);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
