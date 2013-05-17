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
#include "src/logger.h"
#include "src/parameters_base.h"
#include "src/transform_registry.h"

namespace SoundFeatureExtraction {

template <typename FIN, typename FOUT, bool SupportsInversion = false>
class TransformBase;

template <typename FIN, typename FOUT>
class TransformBaseCommon : public virtual Transform,
                            public virtual ParameterizableBase {
  friend class TransformBase<FIN, FOUT, true>;
 public:
  TransformBaseCommon() noexcept
      : inputFormat_(std::make_shared<FIN>()),
        outputFormat_(std::make_shared<FOUT>()) {
  }


  virtual ~TransformBaseCommon() {}

  virtual const std::shared_ptr<BufferFormat> InputFormat() const noexcept {
    if (!IsInverse() || *inputFormat_ == *outputFormat_) {
      return std::static_pointer_cast<BufferFormat>(inputFormat_);
    } else {
      return std::static_pointer_cast<BufferFormat>(outputFormat_);
    }
  }

  virtual void SetInputFormat(const std::shared_ptr<BufferFormat>& format) {
    if (!IsInverse() || *inputFormat_ == *outputFormat_) {
      inputFormat_ = std::static_pointer_cast<FIN>(format);
      OnInputFormatChanged();
    } else {
      outputFormat_ = std::static_pointer_cast<FOUT>(format);
      OnOutputFormatChanged();
    }
  }

  virtual const std::shared_ptr<BufferFormat> OutputFormat() const noexcept {
    if (!IsInverse() || *inputFormat_ == *outputFormat_) {
      return std::static_pointer_cast<BufferFormat>(outputFormat_);
    } else {
      return std::static_pointer_cast<BufferFormat>(inputFormat_);
    }
  }

  virtual void Initialize() const noexcept {
  }

  virtual std::shared_ptr<Buffers> CreateOutputBuffers(
      const Buffers& in) const noexcept {
    assert(&in != nullptr);
    assert(in.Format() != nullptr);
    if (!IsInverse() || *inputFormat_ == *outputFormat_) {
      assert(*in.Format() == *inputFormat_);
      auto buffers = std::make_shared<OutBuffers>(
          std::static_pointer_cast<BufferFormatBase<typename FOUT::BufferType>>(
              outputFormat_));
      InitializeBuffers(reinterpret_cast<const InBuffers&>(in),
                        buffers.get());
      return buffers;
    }
    assert(*in.Format() == *outputFormat_);
    auto buffers = std::make_shared<InBuffers>(
        std::static_pointer_cast<BufferFormatBase<typename FIN::BufferType>>(
            inputFormat_));
    InitializeBuffersInverse(reinterpret_cast<const OutBuffers&>(in),
                             buffers.get());
    return buffers;
  }

  virtual void Do(const Buffers& in, Buffers* out) const noexcept {
    assert(&in != nullptr);
    assert(out != nullptr);
    if (!IsInverse() || *inputFormat_ == *outputFormat_) {
      assert(*in.Format() == *inputFormat_);
      assert(*out->Format() == *outputFormat_);
      Do(reinterpret_cast<const InBuffers&>(in),
         reinterpret_cast<OutBuffers*>(out));
    } else {
      assert(*in.Format() == *outputFormat_);
      assert(*out->Format() == *inputFormat_);
      DoInverse(reinterpret_cast<const OutBuffers&>(in),
                reinterpret_cast<InBuffers*>(out));
    }
  }

 protected:
  std::shared_ptr<FIN> inputFormat_;
  std::shared_ptr<FOUT> outputFormat_;

  typedef BuffersBase<typename FIN::BufferType> InBuffers;
  typedef BuffersBase<typename FOUT::BufferType> OutBuffers;

  virtual void OnInputFormatChanged() {
  }

  virtual void OnOutputFormatChanged() {
  }

  virtual void InitializeBuffers(const InBuffers& in, OutBuffers* out)
      const noexcept = 0;

  virtual void Do(const InBuffers& in, OutBuffers* out)
      const noexcept = 0;

  virtual std::string HostName() const noexcept {
    return std::string("transform ") + Name();
  }

 private:
  virtual void InitializeBuffersInverse(const OutBuffers& in, InBuffers* out)
      const noexcept = 0;

  virtual void DoInverse(const OutBuffers& in, InBuffers* out)
      const noexcept = 0;

  void RegisterInverseParameter() noexcept {
    RegisterSetter("inverse", [&](const std::string& value) {
      if (value != "true" && value != "false") {
        return false;
      }
      return true;
    });
  }
};

template <typename FIN, typename FOUT>
class TransformBase<FIN, FOUT, false> : public TransformBaseCommon<FIN, FOUT> {
 public:
  virtual bool HasInverse() const noexcept {
    return false;
  }

 private:
  virtual void InitializeBuffersInverse(
      const typename TransformBaseCommon<FIN, FOUT>::OutBuffers& in UNUSED,
      typename TransformBaseCommon<FIN, FOUT>::InBuffers* out UNUSED)
      const noexcept {
    std::unexpected();
  }

  virtual void DoInverse(
      const typename TransformBaseCommon<FIN, FOUT>::OutBuffers& in UNUSED,
      typename TransformBaseCommon<FIN, FOUT>::InBuffers* out UNUSED)
      const noexcept {
    std::unexpected();
  }
};

template <typename FIN, typename FOUT>
class TransformBase<FIN, FOUT, true> : public TransformBaseCommon<FIN, FOUT> {
 public:
  TransformBase() {
    this->RegisterInverseParameter();
  }

  virtual bool HasInverse() const noexcept {
    return true;
  }

 protected:
  virtual void InitializeBuffers(
      const typename TransformBaseCommon<FIN, FOUT>::OutBuffers& in,
      typename TransformBaseCommon<FIN, FOUT>::InBuffers* out)
       const noexcept = 0;

  virtual void Do(
      const typename TransformBaseCommon<FIN, FOUT>::OutBuffers& in,
      typename TransformBaseCommon<FIN, FOUT>::InBuffers* out)
      const noexcept = 0;

 private:
  virtual void InitializeBuffersInverse(
      const typename TransformBaseCommon<FIN, FOUT>::OutBuffers& in,
      typename TransformBaseCommon<FIN, FOUT>::InBuffers* out) const noexcept {
    InitializeBuffers(in, out);
  }

  virtual void DoInverse(
      const typename TransformBaseCommon<FIN, FOUT>::OutBuffers& in,
      typename TransformBaseCommon<FIN, FOUT>::InBuffers* out) const noexcept {
    Do(in, out);
  }
};

/// @brief FIN = FOUT case. DO() and InitializeBuffers() overloads become
/// the same then.
template <typename F>
class TransformBase<F, F, true> : public TransformBaseCommon<F, F> {
 public:
  TransformBase() {
    this->RegisterInverseParameter();
  }

  virtual bool HasInverse() const noexcept {
    return true;
  }

 private:
  virtual void InitializeBuffersInverse(
      const typename TransformBaseCommon<F, F>::OutBuffers& in,
      typename TransformBaseCommon<F, F>::InBuffers* out)
      const noexcept {
    this->InitializeBuffers(in, out);
  }

  virtual void DoInverse(
      const typename TransformBaseCommon<F, F>::OutBuffers& in,
      typename TransformBaseCommon<F, F>::InBuffers* out) const noexcept {
    this->Do(in, out);
  }
};

/// @brief Common base of transforms which do not change the buffer format.
template <typename F, bool SupportsInversion = false>
class UniformFormatTransform : public TransformBase<F, F, SupportsInversion> {
 protected:
  virtual void OnInputFormatChanged() {
    this->outputFormat_ = std::make_shared<F>(*this->inputFormat_);
    OnFormatChanged();
  }

  virtual void OnOutputFormatChanged() {
    this->inputFormat_ = std::make_shared<F>(*this->outputFormat_);
    OnFormatChanged();
  }

  virtual void OnFormatChanged() {
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
  static const std::unordered_map<std::string, ParameterTraits> sp = HasInverse()? \
      std::unordered_map<std::string, ParameterTraits> { \
      TP("inverse", \
         "Value indicating whether this transform is inverse.", \
         "false") \
      init \
  } : std::unordered_map<std::string, ParameterTraits> { \
      init \
  }; \
  return sp; \
}

template<class T>
class TransformLogger : public Logger {
 public:
  TransformLogger() : Logger(Logger::Demangle(typeid(T).name()),
                                              EINA_COLOR_LIGHTBLUE) {
  }
};

}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORM_BASE_H_
