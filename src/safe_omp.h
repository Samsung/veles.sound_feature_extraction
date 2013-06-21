/*! @file safe_omp.h
 *  @brief omp.h includer with fallback if OpenMP is not supported.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_SAFE_OMP_H_
#define SRC_SAFE_OMP_H_

#ifdef __clang__
#if __has_include("omp.h")
#define HAVE_OPENMP
#endif
#else
// GCC
#define HAVE_OPENMP
#endif

#if defined(HAVE_OPENMP) && !defined(DEBUG)
#include <omp.h>
#else
inline int omp_get_max_threads() noexcept {
  return 1;
}
#endif

#endif  // SRC_SAFE_OMP_H_
