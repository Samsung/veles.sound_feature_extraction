/*! @file omp_transform_base.h
 *  @brief The base class for transforms which can be executed in parallel.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_OMP_TRANSFORM_BASE_H_
#define SRC_OMP_TRANSFORM_BASE_H_

#include "src/safe_omp.h"
#include "src/transform_base.h"

extern "C" {
  /// @brief Gets the maximal number of threads setting from API.
  /// @note The implementation of this function is in src/api.cc.
  extern int get_omp_transforms_max_threads_num();
}

namespace SoundFeatureExtraction {

template <typename FIN, typename FOUT>
class OmpAwareTransform : public virtual TransformBase<FIN, FOUT> {
 public:
  OmpAwareTransform() noexcept
    : max_number_of_threads_(get_omp_transforms_max_threads_num()) {
    this->RegisterSetter(MaxThreadsNumberParameterName(),
                         [&](const std::string& value) {
      int tn = this->template Parse<int>(this->MaxThreadsNumberParameterName(),
          value);
      if (tn < 1) {
        return false;
      }
      max_number_of_threads_ = tn;
      return true;
    });
  }

  static constexpr const char* MaxThreadsNumberParameterName() noexcept {
    return "threads_num";
  }

  int MaxThreadsNumber() const noexcept {
    int api_max = get_omp_transforms_max_threads_num();
    return max_number_of_threads_ < api_max? max_number_of_threads_ : api_max;
  }

 private:
  int max_number_of_threads_;
};

template <typename F>
class UniformFormatOmpAwareTransform
    : public virtual OmpAwareTransform<F, F>,
      public virtual UniformFormatTransform<F> {
};

template <typename FINELEMENT, typename FOUTELEMENT>
class OmpTransformBaseBufferTypeDispatcher {
 public:
  virtual ~OmpTransformBaseBufferTypeDispatcher() {}

 protected:
  virtual void Do(const FINELEMENT& in, FOUTELEMENT* out) const noexcept = 0;
};

template <typename FINELEMENT, typename FOUTELEMENT>
class OmpTransformBaseBufferTypeDispatcher<FINELEMENT*, FOUTELEMENT> {
 public:
  virtual ~OmpTransformBaseBufferTypeDispatcher() {}

 protected:
  virtual void Do(const FINELEMENT* in, FOUTELEMENT* out) const noexcept = 0;
};

template <typename FINELEMENT, typename FOUTELEMENT>
class OmpTransformBaseBufferTypeDispatcher<FINELEMENT, FOUTELEMENT*> {
 public:
  virtual ~OmpTransformBaseBufferTypeDispatcher() {}

 protected:
  virtual void Do(const FINELEMENT& in, FOUTELEMENT* out) const noexcept = 0;
};

template <typename FINELEMENT, typename FOUTELEMENT>
class OmpTransformBaseBufferTypeDispatcher<FINELEMENT*, FOUTELEMENT*> {
 public:
  virtual ~OmpTransformBaseBufferTypeDispatcher() {}

 protected:
  virtual void Do(const FINELEMENT* in, FOUTELEMENT* out) const noexcept = 0;
};

template <typename FIN, typename FOUT>
class OmpTransformBaseInterface
    : public virtual OmpTransformBaseBufferTypeDispatcher<
          typename FIN::BufferType, typename FOUT::BufferType>,
      public virtual OmpAwareTransform<FIN, FOUT> {
};

template <typename FIN, typename FOUT,
          typename FINELEMENT, typename FOUTELEMENT>
class OmpTransformBaseBufferTypeProxy
    : public virtual OmpTransformBaseInterface<FIN, FOUT> {
 protected:
  using OmpTransformBaseBufferTypeDispatcher<
      typename FIN::BufferType, typename FOUT::BufferType>::Do;

  virtual void Do(
      const typename TransformBase<FIN, FOUT>::InBuffers& in,
      typename TransformBase<FIN, FOUT>::OutBuffers* out)
      const noexcept override final {
#ifdef HAVE_OPENMP
    #pragma omp parallel for num_threads(this->MaxThreadsNumber())
#endif
    for (size_t i = 0; i < in.Count(); i++) {
      this->Do(in[i], &(*out)[i]);
    }
  }
};

template <typename FIN, typename FOUT,
          typename FINELEMENT, typename FOUTELEMENT>
class OmpTransformBaseBufferTypeProxy<FIN, FOUT, FINELEMENT, FOUTELEMENT*>
    : public virtual OmpTransformBaseInterface<FIN, FOUT> {
  protected:
   using OmpTransformBaseBufferTypeDispatcher<
       typename FIN::BufferType, typename FOUT::BufferType>::Do;

   virtual void Do(
       const typename TransformBase<FIN, FOUT>::InBuffers& in,
       typename TransformBase<FIN, FOUT>::OutBuffers* out)
       const noexcept override final {
#ifdef HAVE_OPENMP
    #pragma omp parallel for num_threads(this->MaxThreadsNumber())
#endif
    for (size_t i = 0; i < in.Count(); i++) {
      this->Do(in[i], (*out)[i]);
    }
  }
};

/// @brief So this is what the previous 150+ lines were intended for...
template <typename FIN, typename FOUT>
using OmpTransformBase = OmpTransformBaseBufferTypeProxy<
          FIN, FOUT, typename FIN::BufferType, typename FOUT::BufferType>;

template <typename T>
class OmpInverseTransformBase
    : public virtual OmpTransformBase<typename T::OutFormat,
                                      typename T::InFormat>,
      public virtual InverseTransformBase<T> {
};

/// @brief OpenMP aware transform base class with the same input and output
/// formats.
/// @note Another diamond problem solution here. We take
/// OnFormatChanged(size_t buffersCount) interface simplification from
/// UniformFormatTransform.
template <typename F>
class OmpUniformFormatTransform : public virtual OmpTransformBase<F, F>,
                                  public virtual UniformFormatTransform<F> {
};

template <typename T>
class OmpInverseUniformFormatTransform
    : public virtual InverseTransformBase<T>,
      public virtual OmpUniformFormatTransform<typename T::InFormat> {
};

#define OMP_TRANSFORM_PARAMETERS(init) TRANSFORM_PARAMETERS(FORWARD_MACROS( \
  TP(this->MaxThreadsNumberParameterName(), \
     "The maximal number of OpenMP threads.", \
     std::to_string(get_omp_transforms_max_threads_num())) \
  init) \
)

}  // namespace SoundFeatureExtraction

#endif  // SRC_OMP_TRANSFORM_BASE_H_
