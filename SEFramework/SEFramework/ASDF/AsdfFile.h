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
 * AsdfFile.h
 *
 *  Created on: Aug 14, 2025
 *      Author: embray
 */

#ifndef _SEFRAMEWORK_ASDF_ASDFFILE_H_
#define _SEFRAMEWORK_ASDF_ASDFFILE_H_

#include <boost/filesystem/path.hpp>
#include <asdf.h>

#include "SEFramework/Image/ImageSourceWithMetadata.h"

namespace SourceXtractor {

/**
 * @class AsdfFile
 * @brief represents access to a whole ASDF file
 *
 */
class AsdfFile {
public:
  AsdfFile(const boost::filesystem::path& path);

  AsdfFile(AsdfFile&&) = default;

  virtual ~AsdfFile();

  asdf_file_t* getAsdfFilePtr();

private:
  boost::filesystem::path m_path;
  std::unique_ptr<asdf_file_t, void (*)(asdf_file_t*)> m_asdf_ptr;

  void open();
};

}  // namespace SourceXtractor

#endif /* _SEFRAMEWORK_ASDF_ASDFFILE_H_ */
