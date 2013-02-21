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

#include <assert.h>
#include "src/transform_registry.h"
#include "src/buffers_base.h"

namespace SpeechFeatureExtraction {

template <typename FIN, typename FOUT>
class TransformBase : public virtual Transform {
 public:
  TransformBase(const std::unordered_map<std::string, ParameterTraits>&
                supportedParameters) noexcept {
    for (auto p : supportedParameters) {
      parameters_.insert(std::make_pair(p.first, p.second.DefaultValue));
    }
  }

  virtual ~TransformBase() {}

  virtual const BufferFormat& InputFormat() const noexcept {
    return inputFormat_;
  }

  virtual void SetInputFormat(const BufferFormat& format) {
    inputFormat_ = format;
    OnInputFormatChanged();
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
      parameters_[p.first] = p.second;
      SetParameter(p.first, p.second);
    }
  }

  virtual void Initialize() const noexcept {
  }

  virtual Buffers* CreateOutputBuffers(const Buffers& in) const noexcept {
    assert(in.Format() == inputFormat_);
    auto buffers = new BuffersBase<typename FOUT::BufferType>();
    auto tin = reinterpret_cast<const BuffersBase<
        typename FIN::BufferType>&>(in);
    TypeSafeInitializeBuffers(tin, buffers);
    return buffers;
  }

  virtual void Do(const Buffers& in, Buffers* out) const noexcept {
    assert(in.Format() == inputFormat_);
    assert(out->Format() == outputFormat_);
    auto tin = reinterpret_cast<const BuffersBase<
        typename FIN::BufferType>&>(in);
    auto tout = reinterpret_cast<BuffersBase<
        typename FOUT::BufferType>*>(out);
    TypeSafeDo(tin, tout);
  }

  virtual void DoInverse(const Buffers& in, Buffers* out) const noexcept {
    auto tin = reinterpret_cast<const BuffersBase<
        typename FOUT::BufferType>&>(in);
    auto tout = reinterpret_cast<BuffersBase<
        typename FIN::BufferType>*>(out);
    TypeSafeDoInverse(tin, tout);
  }

 protected:
  FIN inputFormat_;
  FOUT outputFormat_;

  virtual void OnInputFormatChanged() {
  }

  virtual void SetParameter(const std::string& name UNUSED,
                            const std::string& value UNUSED) {
  }

  virtual void TypeSafeInitializeBuffers(
      const BuffersBase<typename FIN::BufferType>& in,
      BuffersBase<typename FOUT::BufferType>* buffers) const noexcept = 0;

  virtual void TypeSafeDo(const BuffersBase<typename FIN::BufferType>& in,
                          BuffersBase<typename FOUT::BufferType>* out)
  const noexcept = 0;

  virtual void TypeSafeDoInverse(
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
  struct identity { typedef T type; };

  int Parse(const std::string& name, const std::string& value,
            identity<int>) {
    int pv;
    try {
      pv = std::stoi(value);
    }
    catch (...) {
      throw new InvalidParameterValueException(name, value, Name());
    }
    return pv;
  }

  size_t Parse(const std::string& name, const std::string& value,
               identity<size_t>) {
    int pv;
    try {
      pv = std::stoul(value);
    }
    catch (...) {
      throw new InvalidParameterValueException(name, value, Name());
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
#endif  // INCLUDE_TRANSFORM_BASE_H_
