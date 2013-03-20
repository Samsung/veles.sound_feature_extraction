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

#ifndef SRC_TRANSFORMS_TEMPLATE_TRANSFORM_H_
#define SRC_TRANSFORMS_TEMPLATE_TRANSFORM_H_

#include <string>
#include "src/formats/window_format.h"
#include "src/uniform_format_transform.h"

namespace SpeechFeatureExtraction {
namespace Transforms {

class TemplateTransform
    : public UniformFormatTransform<Formats::WindowFormatF> {
 public:
  TemplateTransform();

  TRANSFORM_INTRO("!Template", "")

  TRANSFORM_PARAMETERS(
      _TP_("name", "description", "default value")
      _TP_("other_name", "description", "default value")
  )

  virtual void Initialize() const noexcept;

 protected:
  virtual void SetParameter(const std::string& name, const std::string& value);

  virtual void InitializeBuffers(
      const BuffersBase<Formats::WindowF>& in,
      BuffersBase<Formats::WindowF>* buffers) const noexcept;

  virtual void Do(const BuffersBase<Formats::WindowF>& in,
                          BuffersBase<Formats::WindowF> *out) const noexcept;
};

}  // namespace Transforms
}  // namespace SpeechFeatureExtraction
#endif  // SRC_TRANSFORMS_TEMPLATE_TRANSFORM_H_
