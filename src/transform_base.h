/*! @file transform_base.h
 *  @brief Transform template base class.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef TRANSFORM_BASE_H_
#define TRANSFORM_BASE_H_

#include "src/transform_registry.h"

namespace SpeechFeatureExtraction {

template<typename FIN, typename FOUT>
class TransformBase : public Transform {
 public:
  TransformBase(const std::unordered_map<std::string, ParameterTraits>&
                supportedParameters) {
    for (auto p : supportedParameters) {
      parameters_.insert(std::make_pair(p.first, p.second.DefaultValue));
    }
  }

  virtual ~TransformBase() {}

  virtual const std::string& Name() const noexcept = 0;

  virtual const std::unordered_map<std::string, ParameterTraits>&
  SupportedParameters() const noexcept = 0;

  virtual void Initialize() const noexcept = 0;

  virtual void Do(const Buffers& in, Buffers *out) const noexcept = 0;

  virtual BufferFormat* InputFormat() noexcept {
    return &inputFormat_;
  }

  virtual const BufferFormat& OutputFormat() const noexcept {
    return outputFormat_;
  }

  virtual const std::unordered_map<std::string, std::string>&
  CurrentParameters() const noexcept {
    return parameters_;
  }

  virtual void SetParameters(
      const std::unordered_map<std::string, std::string>& parameters)
  throw(InvalidParameterNameException, InvalidParameterValueException) {
    for (auto p : parameters) {
      if (parameters_.find(p.first) == parameters_.end()) {
        throw new InvalidParameterNameException(p.first, Name());
      }
      SetParameter(p.first, p.second);
    }
  }

 protected:
  FIN inputFormat_;
  FOUT outputFormat_;
  std::unordered_map<std::string, std::string> parameters_;

  virtual void SetParameter(const std::string& name, const std::string& value)
  throw(InvalidParameterValueException) = 0;
};

#define TRANSFORM_NAME(name) virtual const std::string& Name() const noexcept { \
    static const std::string str(name); \
    return str; \
}

#define _TP_(name, descr, defval) { name, { descr, defval } },

#define TRANSFORM_PARAMETERS(init) \
    virtual const std::unordered_map<std::string, ParameterTraits>& \
SupportedParameters() const noexcept { \
  static const std::unordered_map<std::string, ParameterTraits> sp = { \
      init \
  }; \
  return sp; \
}

}  // namespace SpeechFeatureExtraction
#endif  // INCLUDE_TRANSFORM_BASE_H_
