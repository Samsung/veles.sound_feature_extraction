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

namespace sound_feature_extraction {

template <typename FIN, typename FOUT>
class OmpAwareTransform : public virtual TransformBase<FIN, FOUT> {
 public:
  OmpAwareTransform() noexcept
    : threads_number_(get_omp_transforms_max_threads_num()) {
  }

  virtual bool BufferInvariant() const noexcept override {
    return true;
  }

  TRANSFORM_PARAMETERS_SUPPORT(FORWARD_MACROS(OmpAwareTransform<FIN, FOUT>))

  TP(threads_number, int, get_omp_transforms_max_threads_num(),
     "The maximal number of OpenMP threads.")
};

template <typename FIN, typename FOUT>
bool OmpAwareTransform<FIN, FOUT>::validate_threads_number(
    const int& value) noexcept {
  return value >= 1;
}

template <typename FIN, typename FOUT>
RTP(FORWARD_MACROS(OmpAwareTransform<FIN, FOUT>), threads_number)

template <typename F>
class UniformFormatOmpAwareTransform
    : public virtual OmpAwareTransform<F, F>,
      public virtual UniformFormatTransform<F> {
};

template <typename FINELEMENT, typename FOUTELEMENT>
class OmpTransformBaseBufferTypeDispatcher {
 public:
#if (__GNUC__ == 4 && __GNUC_MINOR__ < 8)
  virtual ~OmpTransformBaseBufferTypeDispatcher() {};
#else
  virtual ~OmpTransformBaseBufferTypeDispatcher() = default;
#endif

 protected:
  virtual void Do(const FINELEMENT& in, FOUTELEMENT* out) const noexcept = 0;
};

template <typename FINELEMENT, typename FOUTELEMENT>
class OmpTransformBaseBufferTypeDispatcher<FINELEMENT*, FOUTELEMENT> {
 public:
#if (__GNUC__ == 4 && __GNUC_MINOR__ < 8)
  virtual ~OmpTransformBaseBufferTypeDispatcher() {};
#else
  virtual ~OmpTransformBaseBufferTypeDispatcher() = default;
#endif

 protected:
  virtual void Do(const FINELEMENT* in, FOUTELEMENT* out) const noexcept = 0;
};

template <typename FINELEMENT, typename FOUTELEMENT>
class OmpTransformBaseBufferTypeDispatcher<FINELEMENT, FOUTELEMENT*> {
 public:
#if (__GNUC__ == 4 && __GNUC_MINOR__ < 8)
  virtual ~OmpTransformBaseBufferTypeDispatcher() {};
#else
  virtual ~OmpTransformBaseBufferTypeDispatcher() = default;
#endif

 protected:
  virtual void Do(const FINELEMENT& in, FOUTELEMENT* out) const noexcept = 0;
};

template <typename FINELEMENT, typename FOUTELEMENT>
class OmpTransformBaseBufferTypeDispatcher<FINELEMENT*, FOUTELEMENT*> {
 public:
#if (__GNUC__ == 4 && __GNUC_MINOR__ < 8)
  virtual ~OmpTransformBaseBufferTypeDispatcher() {};
#else
  virtual ~OmpTransformBaseBufferTypeDispatcher() = default;
#endif

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
  using OmpTransformBaseInterface<FIN, FOUT>::
      template OmpTransformBaseBufferTypeDispatcher<
          typename FIN::BufferType, typename FOUT::BufferType>::Do;
  using typename OmpTransformBaseInterface<FIN, FOUT>::
      template OmpAwareTransform<FIN, FOUT>::
      template TransformBase<FIN, FOUT>::InBuffers;
  using typename OmpTransformBaseInterface<FIN, FOUT>::
      template OmpAwareTransform<FIN, FOUT>::
      template TransformBase<FIN, FOUT>::OutBuffers;

  virtual void Do(const InBuffers& in, OutBuffers* out)
      const noexcept override final {
#ifdef HAVE_OPENMP
    #pragma omp parallel for num_threads(this->threads_number())
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
  using OmpTransformBaseInterface<FIN, FOUT>::
      template OmpTransformBaseBufferTypeDispatcher<
          typename FIN::BufferType, typename FOUT::BufferType>::Do;
  using typename OmpTransformBaseInterface<FIN, FOUT>::
     template OmpAwareTransform<FIN, FOUT>::
     template TransformBase<FIN, FOUT>::InBuffers;
  using typename OmpTransformBaseInterface<FIN, FOUT>::
     template OmpAwareTransform<FIN, FOUT>::
     template TransformBase<FIN, FOUT>::OutBuffers;

  virtual void Do(const InBuffers& in, OutBuffers* out)
      const noexcept override final {
#ifdef HAVE_OPENMP
    #pragma omp parallel for num_threads(this->threads_number())
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

}  // namespace sound_feature_extraction

#endif  // SRC_OMP_TRANSFORM_BASE_H_
