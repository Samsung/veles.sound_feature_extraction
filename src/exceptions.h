/*! @file Exceptions.h
 *  @brief ExceptionBase class definition.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_EXCEPTIONS_H_
#define SRC_EXCEPTIONS_H_

#include <exception>
#include <string>

/// @brief The general exception class. All other exception classes should
/// inherit from this.
class ExceptionBase : public std::exception {
 public:
  explicit ExceptionBase(const char* message) noexcept
  : message_(message) {}

  explicit ExceptionBase(const std::string& message) noexcept
  : message_(message) {}

  virtual ~ExceptionBase() noexcept {}

  virtual const char* what() const noexcept {
    return message_.c_str();
  }

  virtual const std::string& message() const noexcept {
    return message_;
  }

 private:
  std::string message_;
};


#endif  // SRC_EXCEPTIONS_H_
