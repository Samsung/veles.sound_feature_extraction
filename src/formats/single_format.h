/*! @file single_format.h
 *  @brief Single number format.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_FORMATS_SINGLE_FORMAT_H_
#define SRC_FORMATS_SINGLE_FORMAT_H_

#include "src/buffers_base.h"

namespace SpeechFeatureExtraction {
namespace Formats {

class SingleFormat : public BufferFormatBase<float> {
 public:
  SingleFormat() noexcept {
  }

  SingleFormat(const SingleFormat& other UNUSED) noexcept {
  }

  virtual const std::string& Id() const noexcept {
    static const std::string id("Number");
    return id;
  }

  BufferFormat& operator=(const BufferFormat& other) {
    if (other.Id() != Id()) {
      throw InvalidFormatException(Id(), other.Id());
    }
    *this = reinterpret_cast<const SingleFormat&>(other);
    return *this;
  }

 protected:
  virtual bool MustReallocate(const BufferFormatBase<float>& other UNUSED) {
    return false;
  }
};

}  // namespace Formats
}  // namespace SpeechFeatureExtraction


#endif  // SRC_FORMATS_SINGLE_FORMAT_H_
