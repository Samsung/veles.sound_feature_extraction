/*! @file raw_window_format.h
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

#ifndef RAW_WINDOW_FORMAT_H_
#define RAW_WINDOW_FORMAT_H_

#include "src/buffer_format.h"

namespace SpeechFeatureExtraction {
namespace Formats {

class RawWindowFormat : public BufferFormat {
 public:
  RawWindowFormat();

 protected:
  virtual bool EqualsTo(const BufferFormat& other) const noexcept;
  virtual void SetParametersFrom(const BufferFormat& other) noexcept;
};

}  // namespace Formats
}  // namespace SpeechFeatureExtraction
#endif  // INCLUDE_RAW_WINDOW_FORMAT_H_
