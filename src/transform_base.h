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
#include "src/buffers_base.h"

namespace SpeechFeatureExtraction {

template <typename FINT, typename FOUTT>
class TransformBase : public Transform {
 public:
  TransformBase(const std::unordered_map<std::string, ParameterTraits>&
                supportedParameters) {
    for (auto p : supportedParameters) {
      parameters_.insert(std::make_pair(p.first, p.second.DefaultValue));
    }
  }

  virtual ~TransformBase() {}

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
      const std::unordered_map<std::string, std::string>& parameters) {
    for (auto p : parameters) {
      if (parameters_.find(p.first) == parameters_.end()) {
        throw new InvalidParameterNameException(p.first, Name());
      }
      SetParameter(p.first, p.second);
    }
  }

 protected:
  BufferFormatBase<FINT> inputFormat_;
  FOUTT outputFormat_;
  std::unordered_map<std::string, std::string> parameters_;

  virtual void SetParameter(const std::string& name, const std::string& value)
  = 0;
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

template <typename FINT, typename FOUTT>
class TransformTypeSafeBase
: public TransformBase<BuffersBase<FINT>, BuffersBase<FOUTT>> {

 private:
  typedef TransformBase<BuffersBase<FINT>, BuffersBase<FOUTT>> MyBase;

 public:
  TransformTypeSafeBase(const std::unordered_map<std::string, ParameterTraits>&
                        supportedParameters)
  : MyBase(supportedParameters) {
  }

  virtual ~TransformTypeSafeBase() {}

  virtual void Do(const Buffers& in, Buffers* out) const noexcept {
    auto tin = reinterpret_cast<const BuffersBase<FINT>&>(in);
    auto tout = reinterpret_cast<BuffersBase<FOUTT>*>(out);
    TypeSafeDo(tin, tout);
  }

  virtual void DoInverse(const Buffers& in, Buffers* out) const noexcept {
    auto tin = reinterpret_cast<const BuffersBase<FOUTT>&>(in);
    auto tout = reinterpret_cast<BuffersBase<FINT>*>(out);
    TypeSafeDoInverse(tin, tout);
  }

 protected:
  virtual void TypeSafeDo(const BuffersBase<FINT>& in,
                          BuffersBase<FOUTT>* out) const noexcept = 0;

  virtual void TypeSafeDoInverse(const BuffersBase<FOUTT>& in,
                                 BuffersBase<FINT>* out) const {
    std::unexpected();
  }
};

}  // namespace SpeechFeatureExtraction
#endif  // INCLUDE_TRANSFORM_BASE_H_
