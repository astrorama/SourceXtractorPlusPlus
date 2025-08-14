/** Copyright © 2019 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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

/*
 * AsdfFile.cpp
 *
 *  Created on: Aug 14, 2025
 *      Author: embray
 */

#include <assert.h>

#include <iostream>

#include <boost/filesystem/path.hpp>

#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"

#include "SEFramework/ASDF/AsdfFile.h"

namespace SourceXtractor {

static Elements::Logging logger = Elements::Logging::getLogger("AsdfFile");


AsdfFile::AsdfFile(const boost::filesystem::path& path)
    : m_path(path), m_asdf_ptr(nullptr, asdf_close) {

  open();
}

AsdfFile::~AsdfFile() {}

asdf_file_t* AsdfFile::getAsdfFilePtr() {
  return m_asdf_ptr.get();
}

void AsdfFile::open() {
  asdf_file_t* ptr = asdf_open_file(m_path.native().c_str(), "r");

  // TODO: Actually kinda stinks there's not enough options for retrieving file open errors
  // yet in libasdf; this needs improvement on the libasdf side.
  if (ptr == nullptr) {
      throw Elements::Exception()
          << "Can't open ASDF file: ";
  } else {
    // Check if the file was created but has an error condition set
    const char *error_message = asdf_error(ptr);

    if (error_message != nullptr) {
      throw Elements::Exception()
          << "Can't open ASDF file: " << m_path << " reason: " << error_message;
    }
  }
}

}  // namespace SourceXtractor
