/*! @file format_converter.h
 *  @brief Interface of a converter between formats.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef FORMAT_CONVERTER_H_
#define FORMAT_CONVERTER_H_

#include "src/transform.h"

namespace SpeechFeatureExtraction {

class FormatConverter : public virtual Transform {
 public:
   virtual ~FormatConverter() {}

   virtual const std::string& Name() const noexcept;

   virtual const std::string& Description() const noexcept;

   virtual void SetInputFormat(const BufferFormat& format);

   virtual const std::unordered_map<std::string, ParameterTraits>&
   SupportedParameters() const noexcept;

   virtual const std::unordered_map<std::string, std::string>&
   CurrentParameters() const noexcept;

   virtual void SetParameters(
       const std::unordered_map<std::string, std::string>& parameters);

   virtual void Initialize() const noexcept;

   static std::string Name(const BufferFormat& in,
                           const BufferFormat& out);
};

}  // namespace SpeechFeatureExtraction


#endif  // INCLUDE_FORMAT_CONVERTER_H_
