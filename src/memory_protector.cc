/*! @file memory_protector.cc
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

#include "src/memory_protector.h"
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <unistd.h>
#include <sys/mman.h>

namespace sound_feature_extraction {

MemoryProtector::MemoryProtector(const void* cptr, size_t size) noexcept
    : page_(nullptr), size_(0) {
  // mprotect() requires void*, not const void*
  auto ptr = const_cast<char*>(reinterpret_cast<const char*>(cptr));
  auto rem = reinterpret_cast<uintptr_t>(cptr) % PageSize();
  if (rem > 0) {
    auto offset = PageSize() - rem;
    if (offset > size) {
      // too small memory block, cannot protect
      return;
    }
    ptr += offset;
    size -= offset;
  }
  auto num_pages = size / PageSize();
  if (num_pages < 1) {
    // too small memory block, cannot protect
    return;
  }
  size_ = num_pages * PageSize();
  int res = mprotect(ptr, size_, PROT_READ);
  if (res != 0) {
    fprintf(stderr, "mprotect(%p, %zu, PROT_READ) failed with errno %d\n",
            ptr, size_, errno);
    size_ = 0;
  }
  page_ = ptr;
}

long int MemoryProtector::PageSize() noexcept {
  static auto page_size = sysconf(_SC_PAGE_SIZE);
  return page_size;
}

MemoryProtector::~MemoryProtector() noexcept {
  if (page_ != nullptr) {
    int res = mprotect(page_, size_, PROT_READ | PROT_WRITE);
    if (res != 0) {
    fprintf(stderr, "mprotect(%p, %zu, PROT_READ | PROT_WRITE) failed with "
                    "errno %d\n",
            page_, size_, errno);
    }
  }
}

}  // namespace sound_feature_extraction
