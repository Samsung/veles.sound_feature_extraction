/*! @file transform.h
 *  @brief Transform parent class declaration.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef TRANSFORM_H_
#define TRANSFORM_H_

#include <string>
#include <unordered_map>
#include "src/buffers.h"
#include "src/parameters.h"

namespace SpeechFeatureExtraction {

class InvalidParameterNameException : public ExceptionBase {
 public:
  InvalidParameterNameException(const std::string& name,
                                const std::string& transformName)
  : ExceptionBase("There is no \"" + name + "\" parameter in " +
                  transformName + " supported parameters list.") {}
};

class InvalidParameterValueException : public ExceptionBase {
 public:
  InvalidParameterValueException(const std::string& name,
                                 const std::string& value,
                                 const std::string& transformName)
  : ExceptionBase("Parameter \"" + name + "\" is set to an invalid value \"" +
                  value + "\" for transform " + transformName + ".") {}
};

class Transform {
 public:
  virtual ~Transform();

  virtual const std::string& Name() const noexcept = 0;

  virtual const std::string& DependencyName() const noexcept = 0;

  virtual const std::unordered_map<std::string,
                             std::function<bool(const std::string&)>>&
  DependencyParametersCheck() const noexcept;

  virtual const std::unordered_map<std::string, std::string>&
  DependencyParameters() const noexcept;

  virtual const std::unordered_map<std::string, ParameterTraits>&
  SupportedParameters() const noexcept = 0;

  virtual const std::unordered_map<std::string, std::string>&
  CurrentParameters() const noexcept = 0;

  virtual void SetParameters(
      const std::unordered_map<std::string, std::string>& parameters)
  throw(InvalidParameterNameException, InvalidParameterValueException) = 0;

  virtual void Initialize() const noexcept = 0;

  virtual void Do(const Buffers& in, Buffers *out) const noexcept = 0;

  std::shared_ptr<Transform> Clone() const;

  bool operator==(const Transform& other) const;
};

}  // namespace SpeechFeatureExtraction
#endif  // INCLUDE_TRANSFORM_H_
