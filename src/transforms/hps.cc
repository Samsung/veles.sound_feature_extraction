/*! @file hps.cc
 *  @brief Extracting fundamental frequency with Harmonic Product Spectrum algorithm
 *  @author Shapichev Alexey <a.shapichev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/hps.h"
#include <string>

namespace SoundFeatureExtraction {
namespace Transforms {

const int Hps::kCoefficientsNumber = 3;

void Hps::InitializeBuffers(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<float>* buffers) const noexcept {
  buffers->Initialize(in.Size());
}

void Hps::Do(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<float>* out) const noexcept {
    int sampling_rate = inputFormat_->SamplingRate();
    int length = inputFormat_->Size();
    for (size_t i = 0; i < in.Size(); ++i) {
      auto signal = in[i]->Data.get();
      float max_pitch = 0;
      float fundamental_frequency = 0;
      for (int current_index = 0; (current_index + 1) * kCoefficientsNumber - 1< length;
           ++current_index) {
        float current_pitch = 1.0;
        for (int j = 0; j < kCoefficientsNumber; ++j) {
          current_pitch *= signal[(current_index + 1)* (j + 1) - 1];
        }
        if (current_pitch > max_pitch) {
          max_pitch = current_pitch;
          fundamental_frequency = current_index + 1;
        }
      }
      auto result = (fundamental_frequency * sampling_rate) /
          (2 * (length - 1));
#ifdef DEBUG
      if (result <= 0 || result >= sampling_rate / 2) {
        ERR("Result is out of the allowed range (%f). "
            "Dump of the input data of length %i:",
            result, length);
        std::string dump;
        for (int j = 0; j < length; j++) {
          dump += std::to_string(signal[j]) + "  ";
        }
        ERR("%s", dump.c_str());
      }
#endif
      assert(result > 0 && result < sampling_rate / 2);
      *(*out)[i] = result;
    }
}

REGISTER_TRANSFORM(Hps);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
