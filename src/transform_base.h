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

#ifndef SRC_TRANSFORM_BASE_H_
#define SRC_TRANSFORM_BASE_H_

#include <assert.h>
#include <string>
#include "src/transform_registry.h"
#include "src/buffers_base.h"

namespace SpeechFeatureExtraction {

template <typename FIN, typename FOUT>
class TransformBase : public virtual Transform {
 public:
  TransformBase(const std::unordered_map<std::string, ParameterTraits>&
                supportedParameters) noexcept
      : inputFormat_(std::make_shared<FIN>()),
        outputFormat_(std::make_shared<FOUT>()) {
    for (auto p : supportedParameters) {
      parameters_.insert(std::make_pair(p.first, p.second.DefaultValue));
    }
  }

  virtual ~TransformBase() {}

  virtual const std::shared_ptr<BufferFormat> InputFormat() const noexcept {
    return std::static_pointer_cast<BufferFormat>(inputFormat_);
  }

  virtual void SetInputFormat(const std::shared_ptr<BufferFormat>& format) {
    inputFormat_ = std::static_pointer_cast<FIN>(format);
    OnInputFormatChanged();
  }

  virtual const std::shared_ptr<BufferFormat> OutputFormat() const noexcept {
    return std::static_pointer_cast<BufferFormat>(outputFormat_);
  }

  virtual const std::unordered_map<std::string, std::string>&
  GetParameters() const noexcept {
    return parameters_;
  }

  virtual void SetParameters(
      const std::unordered_map<std::string, std::string>& parameters) {
    for (auto p : parameters) {
      if (parameters_.find(p.first) == parameters_.end()) {
        throw InvalidParameterNameException(p.first, Name());
      }
      parameters_[p.first] = p.second;
      SetParameter(p.first, p.second);
    }
  }

  virtual void Initialize() const noexcept {
  }

  virtual std::shared_ptr<Buffers> CreateOutputBuffers(const Buffers& in) const noexcept {
    assert(*in.Format() == *inputFormat_);
    auto buffers = std::make_shared<BuffersBase<typename FOUT::BufferType>>();
    auto tin = reinterpret_cast<const BuffersBase<  // NOLINT(*)
        typename FIN::BufferType>&>(in);
    InitializeBuffers(tin, buffers.get());
    return buffers;
  }

  virtual void Do(const Buffers& in, Buffers* out) const noexcept {
    assert(*in.Format() == *inputFormat_);
    assert(*out->Format() == *outputFormat_);
    auto tin = reinterpret_cast<const BuffersBase<  // NOLINT(*)
        typename FIN::BufferType>&>(in);
    auto tout = reinterpret_cast<BuffersBase<  // NOLINT(*)
        typename FOUT::BufferType>*>(out);
    Do(tin, tout);
  }

  virtual void DoInverse(const Buffers& in, Buffers* out) const noexcept {
    auto tin = reinterpret_cast<const BuffersBase<  // NOLINT(*)
        typename FOUT::BufferType>&>(in);
    auto tout = reinterpret_cast<BuffersBase<  // NOLINT(*)
        typename FIN::BufferType>*>(out);
    DoInverse(tin, tout);
  }

 protected:
  std::shared_ptr<FIN> inputFormat_;
  std::shared_ptr<FOUT> outputFormat_;

  virtual void OnInputFormatChanged() {
  }

  virtual void SetParameter(const std::string& name UNUSED,
                            const std::string& value UNUSED) {
  }

  virtual void InitializeBuffers(
      const BuffersBase<typename FIN::BufferType>& in,
      BuffersBase<typename FOUT::BufferType>* buffers) const noexcept = 0;

  virtual void Do(const BuffersBase<typename FIN::BufferType>& in,
                          BuffersBase<typename FOUT::BufferType>* out)
  const noexcept = 0;

  virtual void DoInverse(
      const BuffersBase<typename FOUT::BufferType>& in UNUSED,
      BuffersBase<typename FIN::BufferType>* out UNUSED) const {
    std::unexpected();
  }

  template <typename T>
  T Parse(const std::string& name, const std::string& value) {
    return Parse(name, value, identity<T>());
  }

 private:
  std::unordered_map<std::string, std::string> parameters_;

  template<typename T>
  struct identity {
    typedef T type;
  };

  int Parse(const std::string& name, const std::string& value,
            identity<int>) {
    int pv;
    try {
      pv = std::stoi(value);
    }
    catch(...) {
      throw InvalidParameterValueException(name, value, Name());
    }
    return pv;
  }

  size_t Parse(const std::string& name, const std::string& value,
               identity<size_t>) {
    int pv;
    try {
      pv = std::stoul(value);
    }
    catch(...) {
      throw InvalidParameterValueException(name, value, Name());
    }
    return pv;
  }
};

/// @brief Adds Name() and Description() implementations.
/// @param name The name of the transform (as returned by Name()).
/// @param description The description of the transform (as returned
/// by Description()).
#define TRANSFORM_INTRO(name, description) \
virtual const std::string& Name() const noexcept { \
  static const std::string str(name); \
  return str; \
} \
\
virtual const std::string& Description() const noexcept { \
  static const std::string str(description); \
  return str; \
}

#define FORWARD_MACROS(...) __VA_ARGS__

/// @brief Adds a new transform parameter to TRANSFORM_PARAMETERS.
/// @note This macros should be used inside TRANSFORM_PARAMETERS only.
#define _TP_(name, descr, defval) { name, { descr, defval } },

/// @brief Adds SupportedParameters() implementation based on specified
/// parameter items (@see _TP_).
/// @param init The list of _TP_ parameters.
#define TRANSFORM_PARAMETERS(init) \
    virtual const std::unordered_map<std::string, ParameterTraits>& \
SupportedParameters() const noexcept { \
  static const std::unordered_map<std::string, ParameterTraits> sp = { \
      _TP_("inverse", \
           "Value indicating whether this transform is inverse.", \
           "false") \
      init \
  }; \
  return sp; \
}

}  // namespace SpeechFeatureExtraction
#endif  // SRC_TRANSFORM_BASE_H_
