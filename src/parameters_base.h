/*! @file parameters_base.h
 *  @brief Base class which helps to implement IParameterizable.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_PARAMETERS_BASE_H_
#define SRC_PARAMETERS_BASE_H_

#include "src/parameters.h"

namespace SpeechFeatureExtraction {

class InvalidParameterNameException : public ExceptionBase {
 public:
  InvalidParameterNameException(const std::string& name,
                                const std::string& hostName)
  : ExceptionBase("There is no \"" + name + "\" parameter in " +
                  "supported parameters list of " + hostName + ".") {}
};

class InvalidParameterValueException : public ExceptionBase {
 public:
  InvalidParameterValueException(const std::string& name,
                                 const std::string& value,
                                 const std::string& hostName)
  : ExceptionBase("Parameter \"" + name + "\" is set to an invalid value \"" +
                  value + "\" for " + hostName + ".") {}
};

class ParameterSetterNotRegisteredException : public ExceptionBase {
 public:
  ParameterSetterNotRegisteredException(const std::string& name,
                                        const std::string& hostName)
  : ExceptionBase("There is no setter registered for \"" + name +
                  "\" parameter in " + hostName + ".") {}
};

class ParameterizableBase : public virtual IParameterizable {
 public:
  virtual const std::unordered_map<std::string, std::string>&
  GetParameters() const noexcept;

  virtual void SetParameters(
      const std::unordered_map<std::string, std::string>& parameters);

  std::string GetParameter(const std::string& name);

  void SetParameter(const std::string& name, const std::string& value);

 protected:
  typedef std::function<bool(const std::string&)> Setter;  // NOLINT(*)

  void RegisterSetter(const std::string& name, const Setter& setter) noexcept;

  void RegisterSetter(const std::string& name, Setter&& setter) noexcept;

  template <typename T>
  T Parse(const std::string& name, const std::string& value) {
    return Parse(name, value, identity<T>());
  }

  virtual std::string HostName() const noexcept = 0;

 private:
  mutable std::unordered_map<std::string, std::string> values_;
  std::unordered_map<std::string, Setter> setters_;

  void InitializeValues() const noexcept;

  template<typename T>
  struct identity {
    typedef T type;
  };

  bool Parse(const std::string& name, const std::string& value,
             identity<bool>);

  int Parse(const std::string& name, const std::string& value,
            identity<int>);

  size_t Parse(const std::string& name, const std::string& value,
               identity<size_t>);
};

}  // namespace SpeechFeatureExtraction

#endif  // SRC_PARAMETERS_BASE_H_
