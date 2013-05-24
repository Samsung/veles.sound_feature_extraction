/*! @file parameterizable.cc
 *  @brief Parameterizable abstract class partial implementation.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/parameterizable.h"
#include <boost/algorithm/string/trim.hpp>
#include <sstream>

namespace SoundFeatureExtraction {

std::unordered_map<std::string, std::string> Parameterizable::Parse(
    const std::string& line) {
  std::unordered_map<std::string, std::string> parameters;
  std::istringstream ss(line);
  std::string s;
  int index = 0;
  while (std::getline(ss, s, ',')) {
    auto pos = s.find('=');
    if (pos == s.npos) {
      throw ParseParametersException(line, index);
    }
    auto pname = s.substr(0, pos);
    boost::algorithm::trim(pname);
    auto pvalue = s.substr(pos + 1, s.size());
    boost::algorithm::trim(pvalue);
    parameters.insert(std::make_pair(pname, pvalue));
    index += s.size() + 1;
  }
  return std::move(parameters);
}

}  // namespace SoundFeatureExtraction
