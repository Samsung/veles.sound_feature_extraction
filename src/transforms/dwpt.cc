/*! @file dwpt.cc
 *  @brief Discrete wavelet packet transform.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/dwpt.h"
#include <string>

namespace SpeechFeatureExtraction {
namespace Transforms {

DWPT::DWPT()
  : UniformFormatTransform(SupportedParameters()) {
}

void DWPT::Initialize() const noexcept {
}

void DWPT::SetParameter(const std::string& name,
                                     const std::string& value) {
}

void DWPT::TypeSafeInitializeBuffers(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<Formats::WindowF>* buffers) const noexcept {
}

void DWPT::TypeSafeDo(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<Formats::WindowF> *out) const noexcept {
}

REGISTER_TRANSFORM(DWPT);

}  // namespace Transforms
}  // namespace SpeechFeatureExtraction
