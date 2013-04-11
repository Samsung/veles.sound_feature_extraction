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

namespace SoundFeatureExtraction {
namespace Transforms {

TemplateTransform::TemplateTransform() {
  RegisterSetter("name", [&](const std::string& value) {
    int pv = Parse<int>("length", value);

    return true;
  });
  RegisterSetter("other_name", [&](const std::string& value) {
    int pv = Parse<int>("step", value);

    return true;
  });
}

void TemplateTransform::Initialize() const noexcept {
}


void TemplateTransform::InitializeBuffers(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<Formats::WindowF>* buffers) const noexcept {
  buffers->Initialize(in.Size(), inputFormat_->Size());
}

void TemplateTransform::Do(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<Formats::WindowF>* out) const noexcept {
}

REGISTER_TRANSFORM(TemplateTransform);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
