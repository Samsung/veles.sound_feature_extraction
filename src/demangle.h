/*! @file demangle.h
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

#ifndef SRC_DEMANGLE_H_
#define SRC_DEMANGLE_H_

#include <string>

namespace std {

std::string demangle(const char* name);

}  // namespace std

#endif  // SRC_DEMANGLE_H_
