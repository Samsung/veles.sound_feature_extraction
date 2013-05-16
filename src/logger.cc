/*! @file logger.cc
 *  @brief New file description.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/logger.h"
#include <assert.h>
#include <cxxabi.h>
#include <string.h>

namespace SoundFeatureExtraction {

const std::string Logger::kCommonDomain = "sfextr-";
const std::string Logger::kDefaultLoggerColor =
#ifdef EINA
    EINA_COLOR_WHITE
#else
    ""
#endif
    ;

Logger::Logger(const std::string &domain = "default",
               const std::string &color = kDefaultLoggerColor,
               bool suppressLoggingInitialized)
    : log_domain_(kUnintializedLogDomain_)
    , domain_str_(domain)
    , color_(color)
    , suppressLoggingInitialized_(suppressLoggingInitialized) {
#ifdef EINA
  Initialize();
#endif
}

Logger::Logger(const Logger& other)
    : log_domain_(kUnintializedLogDomain_)
    , domain_str_(other.domain_str_)
    , color_(other.color_)
    , suppressLoggingInitialized_(other.suppressLoggingInitialized_) {
#ifdef EINA
  Initialize();
#endif
}

Logger::Logger(Logger&& other)
    : log_domain_(kUnintializedLogDomain_)
    , domain_str_(std::move(std::forward<std::string>(
        other.domain_str_)))
    , color_(std::move(std::forward<std::string>(other.color_)))
    , suppressLoggingInitialized_(
        std::move(std::forward<bool>(
            other.suppressLoggingInitialized_))) {
#ifdef EINA
  Initialize();
#endif
}

Logger& Logger::operator=(const Logger& other) {
  log_domain_ = (kUnintializedLogDomain_)
    , domain_str_ = (other.domain_str_)
    , color_ = (other.color_)
    , suppressLoggingInitialized_ = (other.suppressLoggingInitialized_);
#ifdef EINA
  Initialize();
#endif
  return *this;
}

Logger& Logger::operator=(Logger&& other) {
  log_domain_ = (kUnintializedLogDomain_);
  domain_str_ = (std::move(std::forward<std::string>(
        other.domain_str_)));
  color_ = (std::move(std::forward<std::string>(other.color_)));
  suppressLoggingInitialized_ = (
        std::move(std::forward<bool>(
            other.suppressLoggingInitialized_)));
#ifdef EINA
  Initialize();
#endif
  return *this;
}

Logger::~Logger() {
#ifdef EINA
  Deinitialize();
#endif
}

#ifdef EINA

void Logger::Initialize() {
  Deinitialize();
  eina_init();
  eina_log_threads_enable();
  int len = kCommonDomain.size() + strlen(domain_str_.c_str()) + 1;
  char *fullDomain = new char[len];
  snprintf(fullDomain, len, "%s%s", kCommonDomain.c_str(), domain_str_.c_str());
  log_domain_ = eina_log_domain_register(fullDomain, color_.c_str());
  if (log_domain_ < 0) {
    int message_len = len + 128;
    char *message = new char[message_len];
    snprintf(message, message_len, "%s%s%s",
            "could not register ", fullDomain, " log domain.");
    EINA_LOG_DOM_ERR(EINA_LOG_DOMAIN_GLOBAL, "%s", message);
    log_domain_ = EINA_LOG_DOMAIN_GLOBAL;
  } else {
    if (!suppressLoggingInitialized_) {
      DBG("Logging was initialized with domain %i.",
          log_domain_);
    }
  }
  delete[] fullDomain;
}

void Logger::Deinitialize() {
  if (log_domain_ != kUnintializedLogDomain_ &&
      log_domain_ != EINA_LOG_DOMAIN_GLOBAL) {
    if (!suppressLoggingInitialized_) {
      DBG("Domain %i is not registered now", log_domain_);
    }
    eina_log_domain_unregister(log_domain_);
    log_domain_ = kUnintializedLogDomain_;
  }
}

#endif

int Logger::log_domain() const {
#ifdef EINA
  assert(log_domain_ != kUnintializedLogDomain_);
#endif
  return log_domain_;
}

std::string Logger::domain_str() const {
  return domain_str_;
}

void Logger::set_domain_str(const std::string &value) {
  domain_str_ = value;
#ifdef EINA
  Initialize();
#endif
}

std::string Logger::color() const {
  return color_;
}

void Logger::set_color(const std::string &value) {
  color_ = value;
#ifdef EINA
  Initialize();
#endif
}

std::string Logger::Demangle(const std::string &symbol) {
  size_t size;
  int status;
  char *demangled;
  if ((demangled = abi::__cxa_demangle(symbol.c_str(), NULL, &size, &status))) {
    std::string result(demangled);
    free(demangled);
    return result;
  }

  return symbol;
}

}  // namespace SoundFeatureExtraction
