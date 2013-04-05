/*! @file fork.h
 *  @brief Window cloning.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_FORK_H_
#define SRC_TRANSFORMS_FORK_H_

#include "src/formats/window_format.h"
#include "src/uniform_format_transform.h"

namespace SpeechFeatureExtraction {
namespace Transforms {

class Fork
    : public UniformFormatTransform<Formats::WindowFormatF> {
 public:
  Fork();

  TRANSFORM_INTRO("Fork", "Clones the windows, increasing their number by "
                          "a factor of \"factor\".")

  TRANSFORM_PARAMETERS(
      TP("factor", "Windows number multiplier value.",
         std::to_string(kDefaultFactor))
  )

  static const std::string kCloningFactorAttributeName;

 protected:
  virtual void OnFormatChanged();

  virtual void InitializeBuffers(
      const BuffersBase<Formats::WindowF>& in,
      BuffersBase<Formats::WindowF>* buffers) const noexcept;

  virtual void Do(const BuffersBase<Formats::WindowF>& in,
                  BuffersBase<Formats::WindowF>* out) const noexcept;

 private:
  static const int kDefaultFactor;

  int factor_;
};

}  // namespace Transforms
}  // namespace SpeechFeatureExtraction
#endif  // SRC_TRANSFORMS_FORK_H_
