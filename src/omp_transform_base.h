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

template <typename FINELEMENT, typename FOUTELEMENT,
          bool SupportsInversion = false>
class OmpTransformBaseDoInverseBufferTypeDispatcher {
  public:
   virtual ~OmpTransformBaseDoInverseBufferTypeDispatcher() {}
};

template <typename FINELEMENT, typename FOUTELEMENT>
class OmpTransformBaseDoInverseBufferTypeDispatcher<FINELEMENT,
                                                    FOUTELEMENT,
                                                    true> {
  public:
   virtual ~OmpTransformBaseDoInverseBufferTypeDispatcher() {}

  protected:
   virtual void DoInverse(const FOUTELEMENT& in, FINELEMENT* out)
      const noexcept = 0;
};

template <typename FINELEMENT, typename FOUTELEMENT>
class OmpTransformBaseDoInverseBufferTypeDispatcher<FINELEMENT*,
                                                    FOUTELEMENT,
                                                    true> {
  public:
   virtual ~OmpTransformBaseDoInverseBufferTypeDispatcher() {}

  protected:
   virtual void DoInverse(const FOUTELEMENT& in, FINELEMENT* out)
      const noexcept = 0;
};

template <typename FINELEMENT, typename FOUTELEMENT>
class OmpTransformBaseDoInverseBufferTypeDispatcher<FINELEMENT,
                                                    FOUTELEMENT*,
                                                    true> {
  public:
   virtual ~OmpTransformBaseDoInverseBufferTypeDispatcher() {}

  protected:
   virtual void DoInverse(const FOUTELEMENT* in, FINELEMENT* out)
      const noexcept = 0;
};

template <typename FINELEMENT, typename FOUTELEMENT>
class OmpTransformBaseDoInverseBufferTypeDispatcher<FINELEMENT*,
                                                    FOUTELEMENT*,
                                                    true> {
  public:
   virtual ~OmpTransformBaseDoInverseBufferTypeDispatcher() {}

  protected:
   virtual void DoInverse(const FOUTELEMENT* in, FINELEMENT* out)
      const noexcept = 0;
};

template <typename FINELEMENT, typename FOUTELEMENT>
class OmpTransformBaseDoBufferTypeDispatcher {
 public:
  virtual ~OmpTransformBaseDoBufferTypeDispatcher() {}

 protected:
  virtual void Do(const FINELEMENT& in, FOUTELEMENT* out) const noexcept = 0;
};


template <typename FINELEMENT, typename FOUTELEMENT>
class OmpTransformBaseDoBufferTypeDispatcher<FINELEMENT*, FOUTELEMENT> {
 public:
  virtual ~OmpTransformBaseDoBufferTypeDispatcher() {}

 protected:
  virtual void Do(const FINELEMENT* in, FOUTELEMENT* out) const noexcept = 0;
};

template <typename FINELEMENT, typename FOUTELEMENT>
class OmpTransformBaseDoBufferTypeDispatcher<FINELEMENT, FOUTELEMENT*> {
 public:
  virtual ~OmpTransformBaseDoBufferTypeDispatcher() {}

 protected:
  virtual void Do(const FINELEMENT& in, FOUTELEMENT* out) const noexcept = 0;
};

template <typename FINELEMENT, typename FOUTELEMENT>
class OmpTransformBaseDoBufferTypeDispatcher<FINELEMENT*, FOUTELEMENT*> {
 public:
  virtual ~OmpTransformBaseDoBufferTypeDispatcher() {}

 protected:
  virtual void Do(const FINELEMENT* in, FOUTELEMENT* out) const noexcept = 0;
};

template <typename FINELEMENT, typename FOUTELEMENT,
          bool SupportsInversion>
class OmpTransformBaseBufferTypeDispatcher
    : public virtual OmpTransformBaseDoBufferTypeDispatcher<FINELEMENT,
                                                            FOUTELEMENT>,
      public virtual OmpTransformBaseDoInverseBufferTypeDispatcher<
          FINELEMENT, FOUTELEMENT, SupportsInversion> {
};

template <typename FIN, typename FOUT, bool SupportsInversion = false>
class OmpTransformBaseCommon
    : public virtual OmpTransformBaseBufferTypeDispatcher<
          typename FIN::BufferType, typename FOUT::BufferType,
          SupportsInversion>,
      public virtual TransformBase<FIN, FOUT, SupportsInversion> {
 public:
  OmpTransformBaseCommon() noexcept
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

template <typename FIN, typename FOUT,
          typename FINELEMENT, typename FOUTELEMENT,
          bool SupportsInversion>
class OmpTransformBaseBufferTypeProxy;

template <typename FIN, typename FOUT,
          typename FINELEMENT, typename FOUTELEMENT>
class OmpTransformBaseBufferTypeProxy<FIN, FOUT, FINELEMENT, FOUTELEMENT, false>
    : public virtual OmpTransformBaseCommon<FIN, FOUT, false> {
  protected:
   using OmpTransformBaseBufferTypeDispatcher<
       typename FIN::BufferType, typename FOUT::BufferType, false>::Do;

   virtual void Do(
       const typename TransformBase<FIN, FOUT, false>::InBuffers& in,
       typename TransformBase<FIN, FOUT, false>::OutBuffers* out)
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
class OmpTransformBaseBufferTypeProxy<FIN, FOUT, FINELEMENT, FOUTELEMENT*,
                                      false>
    : public virtual OmpTransformBaseCommon<FIN, FOUT, false> {
  protected:
   using OmpTransformBaseBufferTypeDispatcher<
       typename FIN::BufferType, typename FOUT::BufferType, false>::Do;

   virtual void Do(
       const typename TransformBase<FIN, FOUT, false>::InBuffers& in,
       typename TransformBase<FIN, FOUT, false>::OutBuffers* out)
       const noexcept override final {
#ifdef HAVE_OPENMP
    #pragma omp parallel for num_threads(this->MaxThreadsNumber())
#endif
    for (size_t i = 0; i < in.Count(); i++) {
      this->Do(in[i], (*out)[i]);
    }
  }
};

template <typename FIN, typename FOUT,
          typename FINELEMENT, typename FOUTELEMENT>
class OmpTransformBaseBufferTypeProxy<FIN, FOUT, FINELEMENT, FOUTELEMENT,
                                      true>
    : public virtual OmpTransformBaseCommon<FIN, FOUT, true> {
  protected:
   using OmpTransformBaseBufferTypeDispatcher<
       typename FIN::BufferType, typename FOUT::BufferType, true>::Do;
   using OmpTransformBaseBufferTypeDispatcher<
       typename FIN::BufferType, typename FOUT::BufferType, true>::DoInverse;

   virtual void Do(
       const typename TransformBase<FIN, FOUT, true>::InBuffers& in,
       typename TransformBase<FIN, FOUT, true>::OutBuffers* out)
       const noexcept override final {
#ifdef HAVE_OPENMP
    #pragma omp parallel for num_threads(this->MaxThreadsNumber())
#endif
    for (size_t i = 0; i < in.Count(); i++) {
      this->Do(in[i], &(*out)[i]);
    }
  }

  virtual void DoInverse(
      const typename TransformBase<FIN, FOUT, true>::OutBuffers& in,
      typename TransformBase<FIN, FOUT, true>::InBuffers* out)
      const noexcept override final {
#ifdef HAVE_OPENMP
    #pragma omp parallel for num_threads(this->MaxThreadsNumber())
#endif
    for (size_t i = 0; i < in.Count(); i++) {
      this->DoInverse(in[i], &(*out)[i]);
    }
  }
};

template <typename FIN, typename FOUT,
          typename FINELEMENT, typename FOUTELEMENT>
class OmpTransformBaseBufferTypeProxy<FIN, FOUT, FINELEMENT*, FOUTELEMENT,
                                      true>
    : public virtual OmpTransformBaseCommon<FIN, FOUT, true> {
  protected:
   using OmpTransformBaseBufferTypeDispatcher<
       typename FIN::BufferType, typename FOUT::BufferType, true>::Do;
   using OmpTransformBaseBufferTypeDispatcher<
       typename FIN::BufferType, typename FOUT::BufferType, true>::DoInverse;

   virtual void Do(
       const typename TransformBase<FIN, FOUT, true>::InBuffers& in,
       typename TransformBase<FIN, FOUT, true>::OutBuffers* out)
       const noexcept override final {
#ifdef HAVE_OPENMP
    #pragma omp parallel for num_threads(this->MaxThreadsNumber())
#endif
    for (size_t i = 0; i < in.Count(); i++) {
      this->Do(in[i], &(*out)[i]);
    }
  }

  virtual void DoInverse(
      const typename TransformBase<FIN, FOUT, true>::OutBuffers& in,
      typename TransformBase<FIN, FOUT, true>::InBuffers* out)
      const noexcept override final {
#ifdef HAVE_OPENMP
    #pragma omp parallel for num_threads(this->MaxThreadsNumber())
#endif
    for (size_t i = 0; i < in.Count(); i++) {
      this->DoInverse(in[i], (*out)[i]);
    }
  }
};

template <typename FIN, typename FOUT,
          typename FINELEMENT, typename FOUTELEMENT>
class OmpTransformBaseBufferTypeProxy<FIN, FOUT, FINELEMENT, FOUTELEMENT*,
                                      true>
    : public virtual OmpTransformBaseCommon<FIN, FOUT, true> {
  protected:
   using OmpTransformBaseBufferTypeDispatcher<
       typename FIN::BufferType, typename FOUT::BufferType, true>::Do;
   using OmpTransformBaseBufferTypeDispatcher<
       typename FIN::BufferType, typename FOUT::BufferType, true>::DoInverse;

   virtual void Do(
       const typename TransformBase<FIN, FOUT, true>::InBuffers& in,
       typename TransformBase<FIN, FOUT, true>::OutBuffers* out)
       const noexcept override final {
#ifdef HAVE_OPENMP
    #pragma omp parallel for num_threads(this->MaxThreadsNumber())
#endif
    for (size_t i = 0; i < in.Count(); i++) {
      this->Do(in[i], (*out)[i]);
    }
  }

  virtual void DoInverse(
      const typename TransformBase<FIN, FOUT, true>::OutBuffers& in,
      typename TransformBase<FIN, FOUT, true>::InBuffers* out)
      const noexcept override final {
#ifdef HAVE_OPENMP
    #pragma omp parallel for num_threads(this->MaxThreadsNumber())
#endif
    for (size_t i = 0; i < in.Count(); i++) {
      this->DoInverse(in[i], &(*out)[i]);
    }
  }
};

template <typename FIN, typename FOUT,
          typename FINELEMENT, typename FOUTELEMENT>
class OmpTransformBaseBufferTypeProxy<FIN, FOUT, FINELEMENT*, FOUTELEMENT*,
                                      true>
    : public virtual OmpTransformBaseCommon<FIN, FOUT, true> {
  protected:
   using OmpTransformBaseBufferTypeDispatcher<
       typename FIN::BufferType, typename FOUT::BufferType, true>::Do;
   using OmpTransformBaseBufferTypeDispatcher<
       typename FIN::BufferType, typename FOUT::BufferType, true>::DoInverse;

   virtual void Do(
       const typename TransformBase<FIN, FOUT, true>::InBuffers& in,
       typename TransformBase<FIN, FOUT, true>::OutBuffers* out)
       const noexcept override final {
#ifdef HAVE_OPENMP
    #pragma omp parallel for num_threads(this->MaxThreadsNumber())
#endif
    for (size_t i = 0; i < in.Count(); i++) {
      this->Do(in[i], (*out)[i]);
    }
  }

  virtual void DoInverse(
      const typename TransformBase<FIN, FOUT, true>::OutBuffers& in,
      typename TransformBase<FIN, FOUT, true>::InBuffers* out)
      const noexcept override final {
#ifdef HAVE_OPENMP
    #pragma omp parallel for num_threads(this->MaxThreadsNumber())
#endif
    for (size_t i = 0; i < in.Count(); i++) {
      this->DoInverse(in[i], (*out)[i]);
    }
  }
};

/// @brief So this is what the previous 300+ lines were intended for...
template <typename FIN, typename FOUT, bool SupportsInversion = false>
using OmpTransformBase = OmpTransformBaseBufferTypeProxy<
          FIN, FOUT, typename FIN::BufferType, typename FOUT::BufferType,
          SupportsInversion>;

/// @brief OpenMP aware transform base class with the same input and output
/// formats.
/// @note Another diamond problem solving here. We take OnFormatChanged(size_t buffersCount)
/// interface simplification from UniformFormatTransform.
template <typename F, bool SupportsInversion = false>
class OmpUniformFormatTransform
    : public virtual OmpTransformBase<F, F, SupportsInversion>,
      public virtual UniformFormatTransform<F, SupportsInversion> {
};

#define OMP_TRANSFORM_PARAMETERS(init) TRANSFORM_PARAMETERS(FORWARD_MACROS( \
  TP(this->MaxThreadsNumberParameterName(), \
     "The maximal number of OpenMP threads.", \
     std::to_string(get_omp_transforms_max_threads_num())) \
  init) \
)

}  // namespace SoundFeatureExtraction

#endif  // SRC_OMP_TRANSFORM_BASE_H_
