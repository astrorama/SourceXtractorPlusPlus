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


#include "SEFramework/Image/ImageFileReader.h"
#include "SEImplementation/PythonConfig/PyImageFile.h"


namespace SourceXtractor {

PyImageFile::PyImageFile(const std::string& filename) : m_filename(filename) {
  // Fail here rather than on first use, so a bad filename in a configuration
  // script is reported where the script named it
  ImageFileReader::create(filename);
}


std::string PyImageFile::getFormat() const {
  return ImageFileReader::create(m_filename)->getFormat();
}


std::vector<std::string> PyImageFile::getImageNames() const {
  return ImageFileReader::create(m_filename)->getImageNames();
}


std::shared_ptr<ImageSource> PyImageFile::openImage(const std::string& name) const {
  return ImageFileReader::create(m_filename)->get(name);
}


std::shared_ptr<ImageSource> PyImageFile::openImage(int index) const {
  return ImageFileReader::create(m_filename)->get(index);
}


std::shared_ptr<const Metadata> PyImageFile::getMetadata(const std::string& name) const {
  return openImage(name)->getMetadata();
}


std::shared_ptr<const Metadata> PyImageFile::getMetadataByIndex(int index) const {
  return openImage(index)->getMetadata();
}


Dimensions PyImageFile::getDimensions(const std::string& name) const {
  auto source = openImage(name);
  return {source->getWidth(), source->getHeight()};
}


Dimensions PyImageFile::getDimensionsByIndex(int index) const {
  auto source = openImage(index);
  return {source->getWidth(), source->getHeight()};
}

}  // namespace SourceXtractor
