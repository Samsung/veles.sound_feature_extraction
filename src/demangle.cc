/*! @file demangle.cc
 *  @brief C++ friendly name demangling.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/demangle.h"
#include <cxxabi.h>

namespace std {

std::string demangle(const char* name) {
  int status;
  auto dres = abi::__cxa_demangle(name, nullptr, nullptr, &status);
  if (status == 0) {
    std::string sret(dres);
    free(dres);
    return std::move(sret);
  }
  return std::string(name);
}

}  // namespace std
