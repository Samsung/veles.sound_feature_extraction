/*! @file raw_window_format.h
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

#ifndef RAW_WINDOW_FORMAT_H_
#define RAW_WINDOW_FORMAT_H_

#include "src/buffers_base.h"

namespace SpeechFeatureExtraction {
namespace Formats {

struct Window {

};

class RawWindowFormat : public BufferFormatBase<Window> {
 public:
  RawWindowFormat();
};

}  // namespace Formats
}  // namespace SpeechFeatureExtraction
#endif  // INCLUDE_RAW_WINDOW_FORMAT_H_
