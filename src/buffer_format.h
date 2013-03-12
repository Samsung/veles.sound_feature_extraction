/*! @file buffer_format.h
 *  @brief Buffer format interface class.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_BUFFER_FORMAT_H_
#define SRC_BUFFER_FORMAT_H_

#include <functional>
#include <set>
#include <string>
#include "src/exceptions.h"

namespace SpeechFeatureExtraction {

namespace Formats {
}  // namespace Formats

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
  BufferFormat(const BufferFormat& other) noexcept;
  BufferFormat& operator=(const BufferFormat& other) noexcept;

  virtual ~BufferFormat() noexcept {}

  virtual const std::string& Id() const noexcept;

  bool operator==(const BufferFormat& other) const noexcept;
  bool operator!=(const BufferFormat& other) const noexcept;

  virtual std::function<void(void*)> Destructor() const noexcept = 0;  // NOLINT(*)

 private:
  std::string id_;
};

#define CAST_FORMAT(other, sibling, var) \
    const sibling& var = reinterpret_cast<const sibling&>(other)

}  // namespace SpeechFeatureExtraction
#endif  // SRC_BUFFER_FORMAT_H_
