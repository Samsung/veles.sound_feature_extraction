/*! @file memory.h
 *  @brief Memory routines with SIMD optimization.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_PRIMITIVES_MEMORY_H_
#define SRC_PRIMITIVES_MEMORY_H_

#include <string.h>
#include "spfextr/config.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __AVX__
#include <stdint.h>  // NOLINT(build/include_order)

/// @brief Returns the alignment complement of a pointer to a floating point
/// number array.
/// @param ptr The pointer to a floating point number array.
/// @return If ptr is not aligned,
/// @code
/// (32 - (ptr % 32)) / sizeof(float),
/// @endcode
/// else 0.
int align_complement_f32(const float *ptr) NOTNULL(1);
/// @brief Returns the alignment complement of a pointer to a short integer
/// array.
/// @param ptr The pointer to a short integer array.
/// @return If ptr is not aligned,
/// @code
/// (32 - (ptr % 32)) / sizeof(int16_t),
/// @endcode
/// else 0.
int align_complement_i16(const int16_t *ptr) NOTNULL(1);
/// @brief Returns the alignment complement of a pointer to an integer array.
/// @param ptr The pointer to an integer array.
/// @return If ptr is not aligned,
/// @code
/// (32 - (ptr % 32)) / sizeof(int32_t),
/// @endcode
/// else 0.
int align_complement_i32(const int32_t *ptr) NOTNULL(1);
#endif

/// @brief Allocates an aligned block in the memory.
/// @param size The size of the new block in bytes.
/// @return The newly allocated memory aligned to 32 or 64 bytes (depending on
/// SIMD variant)which should be disposed with free().
void *malloc_aligned(size_t size) MALLOC;

/// @brief Allocates a block in the memory with the specified offset relative
/// to 32 or 64 bytes alignment (depending on SIMD variant).
/// @param size The size of the new block in bytes.
/// @param offset The alignment offset in bytes.
/// @return The newly allocated memory which should be disposed with free().
void *malloc_aligned_offset(size_t size, int offset) MALLOC;

/// @brief Allocates an array of floating point numbers aligned to
/// 32 or 64 bytes (depending on SIMD variant).
/// @param length The length of the block to allocate (in float-s, not
/// in bytes).
float *mallocf(size_t length) MALLOC;

/// @brief Sets the contents of a floating point array to the specified value.
/// @param ptr The array of floating point numbers.
/// @param length The length of the array (in float-s, not in bytes).
/// @param value The value to set for all of the array content.
/// @details Here is an example:
/// @code
/// float array[100];
/// memsetf(array, 100, 1.0f);
/// @endcode
/// So array[i] becomes equal to 1.0f, i = 0..99.
/// @note This function tries to use SIMD instructions available on the host.
void memsetf(float *ptr, size_t length, float value) NOTNULL(1);

/// @brief Allocates a new aligned memory block of size
/// (nearest power of 2 greater than length) * 2, the contents in
/// the difference in lengths being set to zero.
/// @param ptr The array of floating point number which will be zero padded.
/// @param length The length of ptr array (in float-s, not in bytes).
/// @param newLength The pointer to the variable which will hold
/// the new length.
/// @return A newly allocated memory pointer which should be disposed
/// with usual free().
/// @details Here is an example of what this function does. Suggest we have
/// a floating point array of length 100:
/// @code
/// float array[100];
/// @endcode
/// Then we apply zeropadding() function:
/// @code
/// size_t newLength;
/// float *paddedArray = zeropadding(array, 100, &newLength);
/// @endcode
/// Now the following is true:
/// 1. newLength == 256 (since 256 = 2 * 128 and 128 is the nearest power
/// of 2 greater than 100).
/// 2. paddedArray[i] = 0.0f, i = 100..255.
/// 3. paddedArray is aligned to 32 or 64 bytes (depending on SIMD variant).
/// Finally, we free the new array:
/// @code
/// free(paddedArray);
/// @endcode
/// @note This function tries to use SIMD instructions available on the host.
float *zeropadding(const float *ptr, size_t length, size_t *newLength)
NOTNULL(1, 3) MALLOC;

/// @brief Acts like zeropadding(), but it is possible to request some extra
/// space at the end of the newly allocated array, which is too filled
/// with zeros.
/// @param ptr The array of floating point number which will be zero padded.
/// @param length The length of ptr array (in float-s, not in bytes).
/// @param newLength The pointer to the variable which will hold
/// the new length.
/// @param additionalLength Extra length to be allocated. For example,
/// if length is 100, additionalLength is 5, *newLength will be
/// 128 * 2 + 5 = 261 float-s.
/// @return A newly allocated memory pointer which should be disposed
/// with usual free().
/// @note This function tries to use SIMD instructions available on the host.
float *zeropaddingex(const float *ptr, size_t length, size_t *newLength,
                     size_t additionalLength)
NOTNULL(1, 3) MALLOC;

/// @brief Reverse memcpy() for arrays of floating point numbers.
/// That is, dest[i] = src[n - i - 1], i = 0..(n-1).
float *rmemcpyf(float *__restrict dest,
                const float *__restrict src, size_t length) NOTNULL(1, 2);

/// @brief Reverse memcpy() for arrays of floating point complex numbers.
/// That is, dest[i] = src[n - i - 2], i = 0..(n-1), i += 2;
/// dest[i + 1] = src[n - i - 1], i = 1..(n-1), i += 2.
float *crmemcpyf(float *__restrict dest,
                 const float *__restrict src, size_t length) NOTNULL(1, 2);

#ifdef __cplusplus
}
#endif

#endif  // SRC_PRIMITIVES_MEMORY_H_
