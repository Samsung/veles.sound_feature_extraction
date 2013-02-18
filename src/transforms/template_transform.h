/*! @file template_transform.h
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

#ifndef TEMPLATE_TRANSFORM_H_
#define TEMPLATE_TRANSFORM_H_

#include "src/formats/window_format.h"
#include "src/transform_base.h"

namespace SpeechFeatureExtraction {
namespace Transforms {

class TemplateTransform
    : public TransformBase<Formats::WindowFormat, Formats::WindowFormat> {
 public:
  TemplateTransform();

  TRANSFORM_NAME("!Template")

  TRANSFORM_PARAMETERS(
      _TP_("name", "description", "default value")
      _TP_("other_name", "description", "default value")
  )

  virtual void Initialize() const noexcept;

 protected:
  virtual void OnInputFormatChanged();

  virtual void SetParameter(const std::string& name, const std::string& value);

  virtual void TypeSafeInitializeBuffers(const BuffersBase<Formats::Raw16>& in,
        BuffersBase<Formats::Window>* buffers) const noexcept;

  virtual void TypeSafeDo(const BuffersBase<Formats::Raw16>& in,
                          BuffersBase<Formats::Window> *out) const noexcept;
};

}  // namespace Transforms
}  // namespace SpeechFeatureExtraction
#endif  // INCLUDE_TEMPLATE_TRANSFORM_H_
