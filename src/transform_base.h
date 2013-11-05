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
#include "src/parameterizable_base.h"
#include "src/simd_aware.h"
#include "src/transform_registry.h"

namespace sound_feature_extraction {

template <typename FIN, typename FOUT>
class TransformBase : public virtual Transform,
                      public SimdAware,
                      public ParameterizableBase {
 public:
  typedef FIN InFormat;
  typedef FOUT OutFormat;

  TransformBase() noexcept
      : input_format_(std::make_shared<FIN>()),
        output_format_(std::make_shared<FOUT>()) {
  }

  virtual const std::shared_ptr<BufferFormat> InputFormat()
      const noexcept override final {
    return std::static_pointer_cast<BufferFormat>(input_format_);
  }

  virtual size_t SetInputFormat(const std::shared_ptr<BufferFormat>& format,
                                size_t buffersCount) override {
    input_format_ = std::static_pointer_cast<FIN>(format);
    output_format_->CopySourceDetailsFrom(*input_format_);
    return OnInputFormatChanged(buffersCount);
  }

  virtual const std::shared_ptr<BufferFormat> OutputFormat()
      const noexcept override final {
    return std::static_pointer_cast<BufferFormat>(output_format_);
  }

  /// @brief Empty stub. Implement your own, if necessary.
  virtual void Initialize() const override {
  }

  virtual void Do(const Buffers& in, Buffers* out)
      const noexcept override final {
    assert(&in != nullptr);  // yes, this may happen with shared_ptr-s
    assert(out != nullptr);
    assert(*in.Format() == *input_format_);
    assert(*out->Format() == *output_format_);
    this->Do(reinterpret_cast<const InBuffers&>(in),
             reinterpret_cast<OutBuffers*>(out));
  }

  virtual std::shared_ptr<Buffers> CreateOutputBuffers(
      size_t count, void* reusedMemory = nullptr)
      const noexcept override final {
    return std::make_shared<OutBuffers>(output_format_, count, reusedMemory);
  }

 protected:
  std::shared_ptr<FIN> input_format_;
  std::shared_ptr<FOUT> output_format_;

  typedef typename FIN::BufferType InElement;
  typedef typename FOUT::BufferType OutElement;
  typedef BuffersBase<InElement> InBuffers;
  typedef BuffersBase<OutElement> OutBuffers;

  virtual size_t OnInputFormatChanged(size_t buffersCount) {
    return buffersCount;
  }

  virtual size_t OnOutputFormatChanged(size_t buffersCount) {
    return buffersCount;
  }

  virtual void Do(const InBuffers& in, OutBuffers* out)
      const noexcept = 0;

  virtual std::string HostName() const noexcept {
    return std::string("transform ") + Name();
  }
};

/// @brief Common base of transforms which do not change the buffer format.
template <typename F>
class UniformFormatTransform : public virtual TransformBase<F, F> {
 protected:
  virtual size_t OnInputFormatChanged(size_t buffersCount) override final {
    this->output_format_ = std::make_shared<F>(*this->input_format_);
    return OnFormatChanged(buffersCount);
  }

  virtual size_t OnOutputFormatChanged(size_t buffersCount) override final {
    this->input_format_ = std::make_shared<F>(*this->output_format_);
    return OnFormatChanged(buffersCount);
  }

  virtual size_t OnFormatChanged(size_t buffersCount) {
    return buffersCount;
  }
};


template <typename T>
class InverseTransformBase
    : public virtual TransformBase<typename T::OutFormat,
                                   typename T::InFormat> {
 public:
  virtual const std::string& Name() const noexcept override final {
    static const std::string str = std::string("I") + T().Name();
    return str;
  }

  virtual const std::string& Description() const noexcept override final {
    static const std::string str = std::string("Inverse of \"") +
        static_cast<char>(std::tolower(T().Description()[0])) +
        T().Description().substr(1, T().Description().size() - 1) + "\".";
    return str;
  }
};

template <typename T>
class InverseUniformFormatTransform
    : public virtual InverseTransformBase<T>,
      public virtual UniformFormatTransform<typename T::InFormat> {
};

/// @brief Adds Name() and Description() implementations.
/// @param name The name of the transform (as returned by Name()).
/// @param description The description of the transform (as returned
/// by Description()).
#define TRANSFORM_INTRO(name, description) \
virtual const std::string& Name() const noexcept override { \
  static const std::string str(name); \
  return str; \
} \
\
virtual const std::string& Description() const noexcept override { \
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
        SupportedParameters() const noexcept override final { \
      static const std::unordered_map<std::string, ParameterTraits> sp = \
          std::unordered_map<std::string, ParameterTraits> { init };\
      return sp; \
    }

template<class T>
class TransformLogger : public Logger {
 public:
  TransformLogger() noexcept : Logger(std::demangle(typeid(T).name()),
                                      EINA_COLOR_LIGHTBLUE) {
  }
};

}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORM_BASE_H_
