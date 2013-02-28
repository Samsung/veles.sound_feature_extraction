/*! @file format_limits.h
 *  @brief Defines for various format limits.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_FORMATS_FORMAT_LIMITS_H_
#define SRC_FORMATS_FORMAT_LIMITS_H_

#define MIN_SAMPLING_RATE 2000
#define MAX_SAMPLING_RATE 48000
#define DEFAULT_SAMPLING_RATE 16000

#define MIN_WINDOW_DURATION 10
#define MAX_WINDOW_DURATION 100
#define DEFAULT_WINDOW_DURATION 25

#define MIN_WINDOW_STEP 5
#define MAX_WINDOW_STEP 50
#define DEFAULT_WINDOW_STEP 10

#define MIN_RAW_SIZE 128
#define MAX_RAW_SIZE (1 << 30)
#define DEFAULT_SIZE 0

#define MIN_FILTER_LENGTH 10
#define MAX_FILTER_LENGTH 512
#define DEFAULT_FILTER_LENGTH 150

#define MIN_FILTER_FREQUENCY 100
#define MAX_FILTER_FREQUENCY 24000

#define DEFAULT_FILTER_HIGH_FREQUENCY 8000
#define DEFAULT_FILTER_LOW_FREQUENCY 50

#endif  // SRC_FORMATS_FORMAT_LIMITS_H_
