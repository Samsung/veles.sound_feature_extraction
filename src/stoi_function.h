/*! @file stoi_function.h
 *  @brief std::transform friendly std::stoi wrapper.
 *  @author Vadim Markovtsev <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_STOI_FUNCTION_H_
#define SRC_STOI_FUNCTION_H_

#include <functional>
#include <string>

namespace std {

static const auto stoi_function = bind(
    static_cast<int(*)(const string&, size_t*, int)>(&stoi),
    placeholders::_1, nullptr, 10);

}  // namespace std

#endif  // SRC_STOI_FUNCTION_H_
