/*! @file make_unique.h
 *  @brief Overseen std::make_unique implementation. When std gets that function, delete this.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_MAKE_UNIQUE_H_
#define SRC_MAKE_UNIQUE_H_

#include <memory>

namespace std {

template<typename T, typename ...Args>
std::unique_ptr<T> make_unique(Args&& ...args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

}  // namespace std

#endif  // SRC_MAKE_UNIQUE_H_
