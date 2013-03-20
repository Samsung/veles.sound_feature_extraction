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
#include "src/buffers_base.h"
#include "src/parameters_base.h"
#include "src/transform_registry.h"

namespace SpeechFeatureExtraction {

template <typename FIN, typename FOUT>
class TransformBase : public virtual Transform,
                      public virtual ParameterizableBase {
 public:
  TransformBase() noexcept
      : inputFormat_(std::make_shared<FIN>()),
        outputFormat_(std::make_shared<FOUT>()) {
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

  virtual void Initialize() const noexcept {
  }

  virtual std::shared_ptr<Buffers> CreateOutputBuffers(
      const Buffers& in) const noexcept {
    assert(*in.Format() == *inputFormat_);
    auto buffers = std::make_shared<OutBuffers>();
    auto tin = reinterpret_cast<const InBuffers&>(in);
    InitializeBuffers(tin, buffers.get());
    return buffers;
  }

  virtual void Do(const Buffers& in, Buffers* out) const noexcept {
    assert(*in.Format() == *inputFormat_);
    assert(*out->Format() == *outputFormat_);
    auto tin = reinterpret_cast<const InBuffers&>(in);
    auto tout = reinterpret_cast<OutBuffers*>(out);
    Do(tin, tout);
  }

  virtual void DoInverse(const Buffers& in, Buffers* out) const noexcept {
    auto tin = reinterpret_cast<const OutBuffers&>(in);
    auto tout = reinterpret_cast<InBuffers*>(out);
    DoInverse(tin, tout);
  }

 protected:
  std::shared_ptr<FIN> inputFormat_;
  std::shared_ptr<FOUT> outputFormat_;

  typedef BuffersBase<typename FIN::BufferType> InBuffers;
  typedef BuffersBase<typename FOUT::BufferType> OutBuffers;

  virtual void OnInputFormatChanged() {
  }

  virtual void InitializeBuffers(const InBuffers& in,
                                 OutBuffers* out)
      const noexcept = 0;

  virtual void Do(const InBuffers& in,
                  OutBuffers* out)
      const noexcept = 0;

  virtual void DoInverse(const OutBuffers& in UNUSED,
                         InBuffers* out UNUSED) const {
    std::unexpected();
  }

  virtual std::string HostName() const noexcept {
    return std::string("transform ") + Name();
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
#define TP(name, descr, defval) { name, { descr, defval } },

/// @brief Adds SupportedParameters() implementation based on specified
/// parameter items (@see TP).
/// @param init The list of TP parameters.
#define TRANSFORM_PARAMETERS(init) \
    virtual const std::unordered_map<std::string, ParameterTraits>& \
SupportedParameters() const noexcept { \
  static const std::unordered_map<std::string, ParameterTraits> sp = { \
      TP("inverse", \
         "Value indicating whether this transform is inverse.", \
         "false") \
      init \
  }; \
  return sp; \
}

}  // namespace SpeechFeatureExtraction
#endif  // SRC_TRANSFORM_BASE_H_
