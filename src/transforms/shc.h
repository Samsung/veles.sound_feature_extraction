/*! @file shc.h
 *  @brief Prepare for fundamental frequency extraction using Spectral Harmonics Correlation.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_HPS_H_
#define SRC_TRANSFORMS_HPS_H_

#include "src/formats/single_format.h"
#include "src/transforms/common.h"

namespace SoundFeatureExtraction {
namespace Transforms {

class SHCInvalidMinMaxWindowWidthException : public ExceptionBase {
 public:
  explicit SHCInvalidMinMaxWindowWidthException(int min, int max, int window,
                                                int samplingRate)
  : ExceptionBase(std::string("SHC transform was set min=") +
                  std::to_string(min) + ", max=" + std::to_string(max) +
                  " while window width was set to " + std::to_string(window) +
                  " and sampling rate is " + std::to_string(samplingRate) +
                  ".") {
  }
};

/// @brief Prepare for fundamental frequency extraction using Spectral Harmonics
/// Correlation.
class SHC
    : public OmpUniformFormatTransform<Formats::ArrayFormatF>,
      public TransformLogger<SHC> {
 public:
  SHC();

  TRANSFORM_INTRO("SHC", "Apply spectral harmonics correlation.")

  OMP_TRANSFORM_PARAMETERS(
      TP("harmonics", "The number of harmonics.",
         std::to_string(kDefaultHarmonicsNumber))
      TP("window", "The spectral window length, in Hz.",
         std::to_string(kDefaultWindowWidth))
      TP("min", "The minimal frequency to scan.",
         std::to_string(kDefaultMinFrequency))
      TP("max", "The maximal frequency to scan.",
         std::to_string(kDefaultMaxFrequency))
  )

 protected:
  virtual void Initialize() const override;
  virtual size_t OnFormatChanged(size_t buffersCount) override;
  virtual void Do(const float* in, float* out) const noexcept override;

 private:
  static constexpr int kDefaultHarmonicsNumber = 3;
  static constexpr int kDefaultWindowWidth = 40;
  static constexpr int kDefaultMinFrequency = 50;
  static constexpr int kDefaultMaxFrequency = 600;

  int harmonics_;
  int window_width_;
  int min_freq_;
  int max_freq_;
  mutable int half_window_samples_;
  mutable int min_samples_;
  mutable int max_samples_;
};

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_HPS_H_
