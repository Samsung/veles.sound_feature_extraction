/*! @file parameters_base.cc
 *  @brief Base class which helps to implement Parameterizable.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/parameterizable_base.h"

namespace sound_feature_extraction {

const ParametersMap&
ParameterizableBase::GetParameters() const noexcept {
  InitializeValues();
  return values_;
}

void ParameterizableBase::SetParameters(
    const ParametersMap& parameters) {
  for (auto p : parameters) {
    SetParameter(p.first, p.second);
  }
}

std::string ParameterizableBase::GetParameter(const std::string& name) {
  InitializeValues();
  auto it = values_.find(name);
  if (it == values_.end()) {
    throw InvalidParameterNameException(name, HostName());
  }
  return it->second;
}

void ParameterizableBase::SetParameter(const std::string& name,
                                       const std::string& value) {
  InitializeValues();
  if (values_.find(name) == values_.end()) {
    throw InvalidParameterNameException(name, HostName());
  }
  auto setter = ParameterSetters().find(name);
  if (setter == ParameterSetters().end()) {
    throw ParameterSetterNotRegisteredException(name, HostName());
  }
  try {
    setter->second(this, value);
  }
  catch(const InvalidParameterValueException&) {
    throw InvalidParameterValueException(name, value, HostName());
  }
  UpdateParameter(name, value);
}

void ParameterizableBase::UpdateParameter(const std::string& name,
                                          const std::string& value) {
  InitializeValues();
  if (values_.find(name) == values_.end()) {
    throw InvalidParameterNameException(name, HostName());
  }
  values_[name] = value;
}

void ParameterizableBase::InitializeValues() const noexcept {
  if (values_.size() != SupportedParameters().size()) {
    for (auto p : SupportedParameters()) {
      values_.insert(std::make_pair(p.first, p.second.DefaultValue));
    }
  }
}

bool Parse(const std::string& value, identity<bool>) {
  bool ret = (value == "true");
  if (!ret && value != "false") {
    throw InvalidParameterValueException();
  }
  return ret;
}

int Parse(const std::string& value, identity<int>) {
  int pv;
  try {
    pv = std::stoi(value);
  }
  catch(...) {
    throw InvalidParameterValueException();
  }
  return pv;
}

size_t Parse(const std::string& value, identity<size_t>) {
  int pv;
  try {
    pv = std::stoul(value);
  }
  catch(...) {
    throw InvalidParameterValueException();
  }
  return pv;
}

float Parse(const std::string& value, identity<float>) {
  float pv;
  try {
    pv = std::stof(value);
  }
  catch(...) {
    throw InvalidParameterValueException();
  }
  return pv;
}

std::string Parse(const std::string& value, identity<std::string>) {
  return value;
}

}  // namespace sound_feature_extraction
