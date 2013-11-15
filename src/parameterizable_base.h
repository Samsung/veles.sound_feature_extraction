/*! @file parameterizable_base.h
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

#ifndef SRC_PARAMETERIZABLE_BASE_H_
#define SRC_PARAMETERIZABLE_BASE_H_

#include "src/parameterizable.h"

namespace sound_feature_extraction {

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

  InvalidParameterValueException()
  : ExceptionBase("Invalid parameter value.") {}
};

class ParameterSetterNotRegisteredException : public ExceptionBase {
 public:
  ParameterSetterNotRegisteredException(const std::string& name,
                                        const std::string& hostName)
  : ExceptionBase("There is no setter registered for \"" + name +
                  "\" parameter in " + hostName + ".") {}
};

typedef std::function<void(void*, const std::string&)> ParameterSetter;
typedef std::unordered_map<std::string, ParameterSetter> ParameterSettersMap;

class ParameterizableBase : public virtual Parameterizable {
 public:
  virtual const ParametersMap& GetParameters()
      const noexcept;

  virtual void SetParameters(
      const ParametersMap& parameters);

  std::string GetParameter(const std::string& name);

  void SetParameter(const std::string& name, const std::string& value);

 protected:
  void UpdateParameter(const std::string& name, const std::string& value);
  virtual std::string HostName() const noexcept = 0;
  virtual const ParameterSettersMap& ParameterSetters() const noexcept = 0;

 private:
  void InitializeValues() const noexcept;

  mutable ParametersMap values_;
};

template<typename T>
struct identity {
  typedef T type;
};

template <typename T>
static T Parse(const std::string& value) {
  return Parse(value, identity<T>());
}

bool Parse(const std::string& value, identity<bool>);
int Parse(const std::string& value, identity<int>);
size_t Parse(const std::string& value, identity<size_t>);
float Parse(const std::string& value, identity<float>);
std::string Parse(const std::string& value, identity<std::string>);

}  // namespace sound_feature_extraction

namespace std {
  inline string to_string(const string& str) noexcept {
    return str;
  }
}

#endif  // SRC_PARAMETERIZABLE_BASE_H_
