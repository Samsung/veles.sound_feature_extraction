/*! @file logger.h
 *  @brief New file description.
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

#ifndef SRC_LOGGER_H_
#define SRC_LOGGER_H_

#ifdef EINA
#include <Eina.h>
#endif
#include <string>

namespace sound_feature_extraction {

#ifdef EINA

#define DBG(...) EINA_LOG_DOM_DBG(this->log_domain(), __VA_ARGS__)
#define INF(...) EINA_LOG_DOM_INFO(this->log_domain(), __VA_ARGS__)
#define WRN(...) EINA_LOG_DOM_WARN(this->log_domain(), __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(this->log_domain(), __VA_ARGS__)
#define CRT(...) EINA_LOG_DOM_CRIT(this->log_domain(), __VA_ARGS__)

#define DBGI(x, ...) EINA_LOG_DOM_DBG((x)->log_domain(), __VA_ARGS__)
#define INFI(x, ...) EINA_LOG_DOM_INFO((x)->log_domain(), __VA_ARGS__)
#define WRNI(x, ...) EINA_LOG_DOM_WARN((x)->log_domain(), __VA_ARGS__)
#define ERRI(x, ...) EINA_LOG_DOM_ERR((x)->log_domain(), __VA_ARGS__)
#define CRTI(x, ...) EINA_LOG_DOM_CRIT((x)->log_domain(), __VA_ARGS__)

#define DBGC(x, ...) EINA_LOG_DOM_DBG(x::log_domain(), __VA_ARGS__)
#define INFC(x, ...) EINA_LOG_DOM_INFO(x::log_domain(), __VA_ARGS__)
#define WRNC(x, ...) EINA_LOG_DOM_WARN(x::log_domain(), __VA_ARGS__)
#define ERRC(x, ...) EINA_LOG_DOM_ERR(x::log_domain(), __VA_ARGS__)
#define CRTC(x, ...) EINA_LOG_DOM_CRIT(x::log_domain(), __VA_ARGS__)

#else

#include <stdio.h>

#define FALLBACK_LOG(...) { fprintf(stderr, __VA_ARGS__); \
                            fprintf(stderr, "\n"); }

#define DBG(...) FALLBACK_LOG(__VA_ARGS__)
#define INF(...) FALLBACK_LOG(__VA_ARGS__)
#define WRN(...) FALLBACK_LOG(__VA_ARGS__)
#define ERR(...) FALLBACK_LOG(__VA_ARGS__)
#define CRT(...) FALLBACK_LOG(__VA_ARGS__)

#define DBGI(x, ...) FALLBACK_LOG(__VA_ARGS__)
#define INFI(x, ...) FALLBACK_LOG(__VA_ARGS__)
#define WRNI(x, ...) FALLBACK_LOG(__VA_ARGS__)
#define ERRI(x, ...) FALLBACK_LOG(__VA_ARGS__)
#define CRTI(x, ...) FALLBACK_LOG(__VA_ARGS__)

#define DBGC(x, ...) FALLBACK_LOG(__VA_ARGS__)
#define INFC(x, ...) FALLBACK_LOG(__VA_ARGS__)
#define WRNC(x, ...) FALLBACK_LOG(__VA_ARGS__)
#define ERRC(x, ...) FALLBACK_LOG(__VA_ARGS__)
#define CRTC(x, ...) FALLBACK_LOG(__VA_ARGS__)

#define EINA_COLOR_LIGHTRED  ""
#define EINA_COLOR_RED       ""
#define EINA_COLOR_LIGHTBLUE ""
#define EINA_COLOR_BLUE      ""
#define EINA_COLOR_GREEN     ""
#define EINA_COLOR_YELLOW    ""
#define EINA_COLOR_ORANGE    ""
#define EINA_COLOR_WHITE     ""
#define EINA_COLOR_LIGHTCYAN ""
#define EINA_COLOR_CYAN      ""
#define EINA_COLOR_RESET     ""
#define EINA_COLOR_HIGH      ""

#define EINA_LOG_DBG(...) fprintf(stderr, __VA_ARGS__)
#define EINA_LOG_INFO(...) fprintf(stderr, __VA_ARGS__)
#define EINA_LOG_WARN(...) fprintf(stderr, __VA_ARGS__)
#define EINA_LOG_ERR(...) fprintf(stderr, __VA_ARGS__)
#define EINA_LOG_CRIT(...) fprintf(stderr, __VA_ARGS__)

#endif

class Logger {
 public:
  Logger(const std::string &domain = "default",
         const std::string &color = kDefaultLoggerColor,
         bool suppressLoggingInitialized = true) noexcept;

  Logger(const Logger& other) noexcept;

  Logger(Logger&& other) noexcept;

  Logger& operator=(const Logger& other) noexcept;

  Logger& operator=(Logger&& other) noexcept;

  virtual ~Logger();

  int log_domain() const noexcept;

  std::string domain_str() const noexcept;

  void set_domain_str(const std::string &value) noexcept;

  std::string color() const noexcept;

  void set_color(const std::string &value) noexcept;

 private:
  static constexpr const char* kDefaultLoggerColor = EINA_COLOR_WHITE;
  static constexpr const char* kCommonDomain = "";
  static constexpr int kUnintializedLogDomain = -1;

#ifdef EINA
  void InitializeEina() noexcept;
  void DisposeEina() noexcept;
#endif

  int log_domain_;
  std::string domain_str_;
  std::string color_;
  bool suppressLoggingInitialized_;
};

}  // namespace sound_feature_extraction
#endif  // SRC_LOGGER_H_
