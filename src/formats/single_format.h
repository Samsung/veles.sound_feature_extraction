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

template<class T>
class SingleFormat : public BufferFormatBase<T> {
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

  virtual size_t PayloadSizeInBytes() const noexcept {
    return sizeof(T);
  }

 protected:
  virtual bool MustReallocate(const BufferFormatBase<T>& other UNUSED)
      const noexcept {
    return false;
  }

  virtual const void* PayloadPointer(const T& item) const noexcept {
    return &item;
  }

  virtual void Validate(const BuffersBase<T>& buffers) const {
    for (size_t i = 0; i < buffers.Size(); i++) {
      T value = *buffers[i];
      if (value != value) {
        throw InvalidBuffersException(this->Id(), i,
                                      std::to_string(value));
      }
    }
  }
};

typedef SingleFormat<float> SingleFormatF;
typedef SingleFormat<int16_t> SingleFormat16;
typedef SingleFormat<int32_t> SingleForma32;

}  // namespace Formats
}  // namespace SpeechFeatureExtraction


#endif  // SRC_FORMATS_SINGLE_FORMAT_H_
