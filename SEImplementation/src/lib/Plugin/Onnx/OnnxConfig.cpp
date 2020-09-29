/** Copyright © 2020 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "SEImplementation/Plugin/Onnx/OnnxConfig.h"
#include <boost/program_options.hpp>

namespace po = boost::program_options;
using namespace Euclid::Configuration;

namespace SourceXtractor {

static const std::string ONNX_MODEL{"onnx-model"};

OnnxConfig::OnnxConfig(long manager_id) : Configuration(manager_id) {
}

auto OnnxConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"ONNX", {
    {ONNX_MODEL.c_str(), po::value<std::vector<std::string>>()->multitoken(), "ONNX models"}
  }}};
}

void OnnxConfig::initialize(const Configuration::Configuration::UserValues& args) {
  auto i = args.find(ONNX_MODEL);
  if (i != args.end()) {
    m_onnx_model_paths = i->second.as<std::vector<std::string>>();
  }
}

} // end of namespace SourceXtractor
