/*! @file logger.h
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

#ifndef SRC_LOGGER_H_
#define SRC_LOGGER_H_

#ifdef EINA
#include <Eina.h>
#endif
#include <string>

namespace SoundFeatureExtraction {

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

#define FALLBACK_LOG(...) fprintf(stderr, __VA_ARGS__)

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

#endif

class Logger {
 public:
  Logger(const std::string &domain, const std::string &color,
         bool suppressLoggingInitialized = true);

  Logger(const Logger& other);

  Logger(Logger&& other);

  Logger& operator=(const Logger& other);

  Logger& operator=(Logger&& other);

  virtual ~Logger();

  int log_domain() const;

  std::string domain_str() const;

  void set_domain_str(const std::string &value);

  std::string color() const;

  void set_color(const std::string &value);

  static std::string Demangle(const std::string &symbol);

 protected:
  static const int kUnintializedLogDomain_ = -1;
  static const std::string kCommonDomain;
  static const std::string kDefaultLoggerColor;

  int log_domain_;
  std::string domain_str_;
  std::string color_;
  bool suppressLoggingInitialized_;

#ifdef EINA
  void Initialize();
  void Deinitialize();
#endif
};

}  // namespace SoundFeatureExtraction
#endif  // SRC_LOGGER_H_
