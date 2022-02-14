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

#ifndef _SEIMPLEMENTATION_LDACWRITER_H
#define _SEIMPLEMENTATION_LDACWRITER_H

#include <Table/FitsWriter.h>

#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/ImageSource.h"
#include "SEFramework/Frame/Frame.h"
#include "SEFramework/Source/SourceInterface.h"

namespace SourceXtractor {

class LdacWriter : public Euclid::Table::TableWriter {
public:
  ~LdacWriter() override = default;

  LdacWriter(const std::string& filename);

  void addComment(const std::string& comment) override;

  void notifySource(const SourceInterface& source);

protected:
  void writeImHead();

  void init(const Euclid::Table::Table& table) override;

  void append(const Euclid::Table::Table& table) override;

private:
  std::string m_filename;
  std::unique_ptr<Euclid::Table::FitsWriter> m_objects_writer;

  std::vector<std::string> m_comments;
  std::map<std::string, MetadataEntry> m_image_metadata {};
  DetectionImage::PixelType m_rms;
  double m_gain;
};

} // end of namespace SourceXtractor

#endif /* _SEIMPLEMENTATION_LDACWRITER_H */
