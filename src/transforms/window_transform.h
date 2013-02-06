/*! @file window_transform.h
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

#ifndef WINDOWTRANSFORM_H_
#define WINDOWTRANSFORM_H_

#include "src/formats/raw_format.h"
#include "src/formats/raw_window_format.h"
#include "src/transform.h"

namespace SpeechFeatureExtraction {

class WindowTransform : public Transform {
 public:
  WindowTransform();
  virtual ~WindowTransform();

  virtual const std::string& Name() const noexcept {
   static const std::string name("!ROOT!");
   return name;
  }

  virtual BufferFormat* InputFormat() noexcept {
    return &inFormat_;
  }

  virtual const BufferFormat& OutputFormat() const noexcept {
    return outFormat_;
  }

  virtual const std::unordered_map<std::string, ParameterTraits>&
  SupportedParameters() const noexcept {
    static const std::unordered_map<std::string, ParameterTraits> p;
    return p;
  }

  virtual const std::unordered_map<std::string, std::string>&
  CurrentParameters() const noexcept {
    static const std::unordered_map<std::string, std::string> p;
    return p;
  }

  virtual void SetParameters(
      const std::unordered_map<std::string, std::string>&)
  throw(InvalidParameterNameException, InvalidParameterValueException) {
  }

  virtual void Initialize() const noexcept {
  }

  virtual void Do(const Buffers& in, Buffers *out) const noexcept {
    CopyInToOut(in, out);
  }

 private:
  Formats::RawFormat inFormat_;
  Formats::RawWindowFormat outFormat_;
};

}  // namespace SpeechFeatureExtraction
#endif  // INCLUDE_WINDOWTRANSFORM_H_
