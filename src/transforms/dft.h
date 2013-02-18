/*! @file dft.h
 *  @brief Discrete Fourier Transform using FFT.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef TEMPLATE_TRANSFORM_H_
#define TEMPLATE_TRANSFORM_H_

#include <fftf/api.h>
#include "src/formats/window_format.h"
#include "src/transform_base.h"

namespace SpeechFeatureExtraction {
namespace Transforms {

class DFT
    : public TransformBase<Formats::WindowFormat16, Formats::WindowFormatF> {
 public:
  DFT();

  TRANSFORM_NAME("DFT")

  TRANSFORM_PARAMETERS()

  virtual void Initialize() const noexcept;

 protected:
  virtual void OnInputFormatChanged();

  virtual void SetParameter(const std::string& name, const std::string& value);

  virtual void TypeSafeInitializeBuffers(const BuffersBase<Formats::Window16>& in,
        BuffersBase<Formats::WindowF>* buffers) const noexcept;

  virtual void TypeSafeDo(const BuffersBase<Formats::Window16>& in,
                          BuffersBase<Formats::WindowF> *out) const noexcept;

 private:
  std::vector<float> buffer_;
  mutable std::shared_ptr<FFTFInstance> fftPlan_;
};

}  // namespace Transforms
}  // namespace SpeechFeatureExtraction
#endif  // INCLUDE_TEMPLATE_TRANSFORM_H_
