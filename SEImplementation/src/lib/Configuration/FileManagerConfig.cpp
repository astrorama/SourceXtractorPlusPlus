/** Copyright © 2019-2025 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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

#include "SEImplementation/Configuration/FileManagerConfig.h"

#include <boost/algorithm/string.hpp>

#include "ElementsKernel/Exception.h"
#include "Configuration/ConfigManager.h"

#include "SEFramework/FITS/FitsImageSource.h"

#include <iostream>

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SourceXtractor {

static const std::string MAX_SIMULTANEOUS_FILES {"max-simultaneous-files" };


 // Constructor
FileManagerConfig::FileManagerConfig(long manager_id)
 : Configuration(manager_id), m_max_simultaneous_files(500) {
}

std::map<std::string, Configuration::OptionDescriptionList> FileManagerConfig::getProgramOptions() {
    std::cout << "Getting FileManagerConfig options" << std::endl;
    return { {"File Manager", {
        {MAX_SIMULTANEOUS_FILES.c_str(), po::value<int>()->default_value(500),
            "Maximum number of simultaneous open files."}
    }} };
}

void FileManagerConfig::preInitialize(const UserValues& args) {
    // We have to configure the FileManager before any other configuration tries to use it
    m_max_simultaneous_files = args.find(MAX_SIMULTANEOUS_FILES)->second.as<int>();
    FitsImageSource::getFileManager(m_max_simultaneous_files);
}

void FileManagerConfig::initialize(const UserValues& args) {
}

} // namespace SourceXtractor