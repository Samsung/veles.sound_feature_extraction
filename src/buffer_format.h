/*! @file buffer_format.h
 *  @brief Buffer format base class.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef BUFFER_FORMAT_H_
#define BUFFER_FORMAT_H_

#include <set>
#include <string>
#include "src/exceptions.h"

namespace SpeechFeatureExtraction {

class InvalidFormatException : public ExceptionBase {
 public:
  InvalidFormatException(const std::string& idMine,
                         const std::string& idYours)
  : ExceptionBase("Attempted to copy parameters for format \"" + idMine +
                  "\" from format \"" + idYours + "\".") {}
};

class BufferFormat {
 public:
  explicit BufferFormat(const std::string& id) noexcept;
  explicit BufferFormat(const BufferFormat& other) = delete;
  virtual ~BufferFormat() noexcept {}
  const std::string& Id() const noexcept;

  BufferFormat& operator=(const BufferFormat& other)
  throw (InvalidFormatException);
  bool operator==(const BufferFormat& other) const noexcept;

 protected:
  virtual bool EqualsTo(const BufferFormat& other) const noexcept = 0;
  virtual void SetParametersFrom(const BufferFormat& other) noexcept = 0;

 private:
  std::string id_;
};

}  // namespace SpeechFeatureExtraction
#endif  // INCLUDE_BUFFER_FORMAT_H_
