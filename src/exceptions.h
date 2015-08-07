/*! @file Exceptions.h
 *  @brief ExceptionBase class definition.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright © 2013 Samsung R&D Institute Russia
 *
 *  @section License
 *  Licensed to the Apache Software Foundation (ASF) under one
 *  or more contributor license agreements.  See the NOTICE file
 *  distributed with this work for additional information
 *  regarding copyright ownership.  The ASF licenses this file
 *  to you under the Apache License, Version 2.0 (the
 *  "License"); you may not use this file except in compliance
 *  with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing,
 *  software distributed under the License is distributed on an
 *  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 *  KIND, either express or implied.  See the License for the
 *  specific language governing permissions and limitations
 *  under the License.
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

  virtual ~ExceptionBase() noexcept {
  }

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
