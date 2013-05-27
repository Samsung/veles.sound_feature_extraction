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

#include <omp.h>
#include "src/transform_base.h"

extern "C" {
  /// @brief Gets the maximal number of threads setting from API.
  /// @note The implementation of this function is in src/api.cc.
  extern int get_omp_transforms_max_threads_num();
}

namespace SoundFeatureExtraction {

template <typename FIN, typename FOUT>
class OmpTransformBaseCommon : public virtual ParameterizableBase {
 public:
  OmpTransformBaseCommon() noexcept
    : max_number_of_threads_(get_omp_transforms_max_threads_num()) {
    RegisterSetter(MaxThreadsNumberParameterName(),
                   [&](const std::string& value) {
      int tn = Parse<int>(MaxThreadsNumberParameterName(), value);
      if (tn < 1) {
        return false;
      }
      int api_max = get_omp_transforms_max_threads_num();
      max_number_of_threads_ = tn < api_max? tn : api_max;
      return true;
    });
  }

  static constexpr std::string MaxThreadsNumberParameterName() {
    return "threads_num";
  }

  int MaxThreadsNumber() const noexcept {
    return max_number_of_threads_;
  }

 protected:
  typedef typename FIN::BufferType InElement;
  typedef typename FOUT::BufferType OutElement;
  typedef BuffersBase<InElement> InBuffers;
  typedef BuffersBase<OutElement> OutBuffers;

  virtual void Do(const InBuffers& in, OutBuffers* out) const noexcept {
    #pragma omp parallel for num_threads(max_number_of_threads_)
    for (size_t i = 1; i < in.Size(); i++) {
      Do(in[i], &(*out)[i]);
    }
  }

  virtual void Do(const InElement& in, OutElement* out) const noexcept = 0;

 private:
  virtual void DoInverse(const OutBuffers& in, InBuffers* out) const noexcept {
    #pragma omp parallel for num_threads(max_number_of_threads_)
    for (size_t i = 1; i < in.Size(); i++) {
      DoInverse(in[i], &(*out)[i]);
    }
  }

  virtual void DoInverse(const OutElement& in, InElement* out)
  const noexcept = 0;

  int max_number_of_threads_;
};

template <typename FIN, typename FOUT, bool SupportsInversion = false>
class OmpTransformBase;

template <typename FIN, typename FOUT>
class OmpTransformBase<FIN, FOUT, false>
    : public virtual TransformBase<FIN, FOUT, false>,
      public virtual OmpTransformBaseCommon<FIN, FOUT> {
 protected:
  virtual void Do(const typename TransformBase<FIN, FOUT, false>::InBuffers& in,
                  typename TransformBase<FIN, FOUT, false>::OutBuffers* out)
  const noexcept {
    OmpTransformBaseCommon<FIN, FOUT>::Do(in, out);
  }

 private:
  virtual void DoInverse(
      const typename OmpTransformBaseCommon<FIN, FOUT>::OutElement& in UNUSED,
      typename OmpTransformBaseCommon<FIN, FOUT>::InElement* out UNUSED)
      const noexcept {
    std::unexpected();
  }
};

template <typename FIN, typename FOUT>
class OmpTransformBase<FIN, FOUT, true>
    : public virtual TransformBase<FIN, FOUT, true>,
      public virtual OmpTransformBaseCommon<FIN, FOUT> {
 protected:
  virtual void Do(const typename TransformBase<FIN, FOUT, true>::InBuffers& in,
                  typename TransformBase<FIN, FOUT, true>::OutBuffers* out)
  const noexcept {
    OmpTransformBaseCommon<FIN, FOUT>::Do(in, out);
  }

  virtual void Do(const typename TransformBase<FIN, FOUT, true>::OutBuffers& in,
                  typename TransformBase<FIN, FOUT, true>::InBuffers* out)
  const noexcept {
    OmpTransformBaseCommon<FIN, FOUT>::DoInverse(in, out);
  }

  virtual void Do(
      const typename OmpTransformBaseCommon<FIN, FOUT>::OutElement& in,
      typename OmpTransformBaseCommon<FIN, FOUT>::InElement* out)
      const noexcept = 0;

 private:
  virtual void DoInverse(
      const typename OmpTransformBaseCommon<FIN, FOUT>::OutElement& in,
      typename OmpTransformBaseCommon<FIN, FOUT>::InElement* out)
  const noexcept {
    Do(in, out);
  }
};

template <typename F>
class OmpTransformBase<F, F, true>
    : public virtual TransformBase<F, F, true>,
      public virtual OmpTransformBaseCommon<F, F> {
 protected:
  virtual void Do(const typename TransformBase<F, F, true>::InBuffers& in,
                  typename TransformBase<F, F, true>::OutBuffers* out)
  const noexcept {
    OmpTransformBaseCommon<F, F>::Do(in, out);
  }

 private:
  virtual void DoInverse(
      const typename OmpTransformBaseCommon<F, F>::OutElement& in,
      typename OmpTransformBaseCommon<F, F>::InElement* out) const noexcept {
    this->Do(in, out);
  }
};

template <typename F, bool SupportsInversion = false>
using OmpUniformFormatTransform = OmpTransformBase<F, F, SupportsInversion>;

#define OMP_TRANSFORM_PARAMETERS(init) TRANSFORM_PARAMETERS(FORWARD_MACROS( \
  TP(MaxThreadsNumberParameterName(), \
     "The maximal number of OpenMP threads.", \
     "get_omp_transforms_max_threads_num()") \
  init) \
)

}  // namespace SoundFeatureExtraction

#endif  // SRC_OMP_TRANSFORM_BASE_H_
