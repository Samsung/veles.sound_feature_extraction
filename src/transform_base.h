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
#include "src/simd_aware.h"
#include "src/transform_registry.h"

namespace SoundFeatureExtraction {

template <class INBUFFERS, class OUTBUFFERS, bool SupportsInversion = false>
class DoInverseConditionalDeclarator {
 public:
  virtual ~DoInverseConditionalDeclarator() {}

 protected:
  void DoInverse(const OUTBUFFERS&, INBUFFERS*)
      const noexcept {
    std::unexpected();
  }
};

template <class INBUFFERS, class OUTBUFFERS>
class DoInverseConditionalDeclarator<INBUFFERS, OUTBUFFERS, true> {
 public:
  virtual ~DoInverseConditionalDeclarator() {}

 protected:
  virtual void DoInverse(const OUTBUFFERS& in, INBUFFERS* out)
      const noexcept = 0;
};

template <typename FIN, typename FOUT, bool SupportsInversion = false>
class TransformBase : public virtual Transform,
                      public virtual SimdAware,
                      public virtual InverseParameterAware,
                      public virtual ParameterizableBase,
                      public virtual DoInverseConditionalDeclarator<
                          BuffersBase<typename FIN::BufferType>,
                          BuffersBase<typename FOUT::BufferType>,
                          SupportsInversion> {
 public:
  typedef FIN InFormat;
  typedef FOUT OutFormat;

  TransformBase() noexcept
      : inputFormat_(std::make_shared<FIN>()),
        outputFormat_(std::make_shared<FOUT>()) {
    if (SupportsInversion) {
      RegisterSetter(kInverseParameterName, [&](const std::string& value) {
        if (value != "true" && value != "false") {
          return false;
        }
        return true;
      });
    }
  }

  virtual const std::shared_ptr<BufferFormat> InputFormat()
      const noexcept override final {
    if (!IsInverse() || std::is_same<FIN, FOUT>::value) {
      return std::static_pointer_cast<BufferFormat>(inputFormat_);
    } else {
      return std::static_pointer_cast<BufferFormat>(outputFormat_);
    }
  }

  virtual size_t SetInputFormat(
      const std::shared_ptr<BufferFormat>& format,
      size_t buffersCount) override {
    if (!IsInverse() || std::is_same<FIN, FOUT>::value) {
      inputFormat_ = std::static_pointer_cast<FIN>(format);
      outputFormat_->CopySourceDetailsFrom(*inputFormat_);
      return OnInputFormatChanged(buffersCount);
    } else {
      outputFormat_ = std::static_pointer_cast<FOUT>(format);
      inputFormat_->CopySourceDetailsFrom(*outputFormat_);
      return OnOutputFormatChanged(buffersCount);
    }
  }

  virtual const std::shared_ptr<BufferFormat> OutputFormat()
      const noexcept override final {
    if (!IsInverse() || std::is_same<FIN, FOUT>::value) {
      return std::static_pointer_cast<BufferFormat>(outputFormat_);
    } else {
      return std::static_pointer_cast<BufferFormat>(inputFormat_);
    }
  }

  virtual void Initialize() const noexcept override {
  }

  virtual void Do(const Buffers& in, Buffers* out)
      const noexcept override final {
    assert(&in != nullptr);
    assert(out != nullptr);
    if (!IsInverse()) {
      assert(*in.Format() == *inputFormat_);
      assert(*out->Format() == *outputFormat_);
      this->Do(reinterpret_cast<const InBuffers&>(in),
         reinterpret_cast<OutBuffers*>(out));
    } else {
      assert(*in.Format() == *outputFormat_);
      assert(*out->Format() == *inputFormat_);
      this->DoInverse(reinterpret_cast<const OutBuffers&>(in),
                reinterpret_cast<InBuffers*>(out));
    }
  }

  virtual std::shared_ptr<Buffers> CreateOutputBuffers(
      size_t count, void* reusedMemory = nullptr)
      const noexcept override final {
    return std::make_shared<OutBuffers>(outputFormat_, count, reusedMemory);
  }

  bool IsInverse() const noexcept {
    auto ip = GetParameters().find(kInverseParameterName);
    if (ip == GetParameters().end()) {
      return false;
    }
    return ip->second == "true";
  }

  static constexpr bool HasInverseTransform() noexcept {
    return SupportsInversion;
  }

 protected:
  std::shared_ptr<FIN> inputFormat_;
  std::shared_ptr<FOUT> outputFormat_;

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
template <typename F, bool SupportsInversion = false>
class UniformFormatTransform
    : public virtual TransformBase<F, F, SupportsInversion> {
 protected:
  virtual size_t OnInputFormatChanged(size_t buffersCount) override final {
    this->outputFormat_ = std::make_shared<F>(*this->inputFormat_);
    return OnFormatChanged(buffersCount);
  }

  virtual size_t OnOutputFormatChanged(size_t buffersCount) override final {
    this->inputFormat_ = std::make_shared<F>(*this->outputFormat_);
    return OnFormatChanged(buffersCount);
  }

  virtual size_t OnFormatChanged(size_t buffersCount) {
    return buffersCount;
  }
};


/// @brief Adds Name() and Description() implementations.
/// @param name The name of the transform (as returned by Name()).
/// @param description The description of the transform (as returned
/// by Description()).
#define TRANSFORM_INTRO(name, description) \
virtual const std::string& Name() const noexcept override final { \
  static const std::string str(name); \
  return str; \
} \
\
virtual const std::string& Description() const noexcept override final { \
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
          this->HasInverseTransform()? \
              std::unordered_map<std::string, ParameterTraits> { \
                TP(this->kInverseParameterName, \
                  "Value indicating whether this transform is inverse.", \
                  "false") \
                init \
              } \
          : std::unordered_map<std::string, ParameterTraits> { init };\
      return sp; \
    }

template<class T>
class TransformLogger : public Logger {
 public:
  TransformLogger() noexcept : Logger(std::demangle(typeid(T).name()),
                                      EINA_COLOR_LIGHTBLUE) {
  }
};

}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORM_BASE_H_
