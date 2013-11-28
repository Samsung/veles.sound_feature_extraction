/*! @file memory_protector.h
 *  @brief Write memory protection based on mprotect().
 *  @author Vadim Markovtsev <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_MEMORY_PROTECTOR_H_
#define SRC_MEMORY_PROTECTOR_H_

#include <cstddef>

namespace sound_feature_extraction {

class MemoryProtector {
 public:
  MemoryProtector(const void* cptr, size_t size) noexcept;
  ~MemoryProtector() noexcept;

  void* page() const noexcept;
  size_t size() const noexcept;

 protected:
  static long int PageSize() noexcept;

 private:
  void* page_;
  size_t size_;
};

}  // namespace sound_feature_extraction
#endif  // SRC_MEMORY_PROTECTOR_H_
