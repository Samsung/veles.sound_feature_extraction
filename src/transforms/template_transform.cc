/*! @file template_transform.cc
 *  @brief New file description.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/template_transform.h"
#include <string>

namespace SpeechFeatureExtraction {
namespace Transforms {

TemplateTransform::TemplateTransform()
: TransformBase(SupportedParameters()) {
}

void TemplateTransform::Initialize() const noexcept {
}

void TemplateTransform::OnInputFormatChanged() {
}

void TemplateTransform::SetParameter(const std::string& name,
                                     const std::string& value) {
}

void TemplateTransform::TypeSafeInitializeBuffers(
    const BuffersBase<Formats::Raw16>& in,
    BuffersBase<Formats::Window>* buffers) const noexcept {
}

void TemplateTransform::TypeSafeDo(
    const BuffersBase<Formats::Raw16>& in,
    BuffersBase<Formats::Window> *out) const noexcept {
}

REGISTER_TRANSFORM(TemplateTransform);

}  // namespace Transforms
}  // namespace SpeechFeatureExtraction
