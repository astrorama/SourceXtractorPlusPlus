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

#include <AlexandriaKernel/memory_tools.h>
#include "SEFramework/Property/DetectionFrame.h"
#include "SEImplementation/Output/LdacWriter.h"
#include "SEImplementation/Configuration/DetectionImageConfig.h"
#include "SOURCEXTRACTORPLUSPLUS_VERSION.h"

namespace SourceXtractor {

using Euclid::Configuration::ConfigManager;
using Euclid::make_unique;
using namespace Euclid::Table;


LdacWriter::LdacWriter(const std::string& filename, ConfigManager& manager)
  : m_config_manager(manager), m_filename(filename) {
}

void LdacWriter::addComment(const std::string& comment) {
  if (m_objects_writer) {
    throw Elements::Exception() << "Can not add comments once the output table has been initialized";
  }
  m_comments.emplace_back(comment);
}

// Handle sources instead of table records, so before writing anything
// we can recover useful information
void LdacWriter::notifySource(const SourceInterface& source) {
  if (m_objects_writer)
    return;

  const auto& detection_frame = source.getProperty<DetectionFrame>();
  m_rms = detection_frame.getFrame()->getBackgroundMedianRms();
}

template<typename T>
static std::string generateHeader(const std::string& name, T value, const std::string& comment) {
  std::stringstream str;

  str << std::setw(8) << std::left << name << "= "
      << std::scientific << std::setw(20) << std::right << value
      << " / "
      << std::setw(47) << std::left << comment;
  assert(str.str().size() == 80);

  return str.str();
}

static void generateHistory(std::vector<std::string>& headers) {
  std::vector<std::stringstream> entries;

  entries.emplace_back();
  entries.back() << "Version " << SOURCEXTRACTORPLUSPLUS_VERSION_STRING;

  auto t = std::time(nullptr);
  auto now = *std::gmtime(&t);

  entries.emplace_back();
  entries.back() << "Called at " << std::put_time(&now, "%Y-%m-%dT%H:%M:%SZ");

  for (auto& e : entries) {
    std::stringstream padder;
    padder << "HISTORY s++: " << std::setw(67) << std::left << e.str();
    headers.emplace_back(padder.str());
  }
}

void LdacWriter::writeImHead() {
  auto& detection_image_config = m_config_manager.getConfiguration<DetectionImageConfig>();
  auto imhead_writer = make_unique<FitsWriter>(m_filename, true);
  imhead_writer->setHduName("LDAC_IMHEAD");

  // Headers from the image
  std::vector<std::string> ldac_imhead;
  auto img_source = detection_image_config.getImageSource();
  int n_headers;
  auto img_headers = img_source->getFitsHeaders(n_headers);
  for (int i = 0; i < n_headers; ++i) {
    ldac_imhead.emplace_back(&(*img_headers)[80 * i], 80);
  }

  // Headers from the configuration and detection
  auto gain = detection_image_config.getGain();
  ldac_imhead.emplace_back(generateHeader("SEXGAIN", gain, "Gain used"));
  ldac_imhead.emplace_back(generateHeader("SEXBKDEV", m_rms, "Median background RMS"));
  ldac_imhead.emplace_back("END" + std::string(77, ' '));

  // History, why not
  generateHistory(ldac_imhead);

  // Write the single row
  auto column_info = std::make_shared<ColumnInfo>(std::vector<ColumnInfo::info_type>{
    {"Field Header Card", typeid(std::vector<std::string>)}
  });
  Row row{std::vector<Row::cell_type>{ldac_imhead}, column_info};

  imhead_writer->addData(Table{std::vector<Row>{row}});
}

void LdacWriter::init(const Table&) {
  // Initialize LDAC_IMHEAD HDU
  writeImHead();

  // Initialize LDAC_OBJECTS HDU
  m_objects_writer = make_unique<FitsWriter>(m_filename, false);
  m_objects_writer->setHduName("LDAC_OBJECTS");
  for (auto& comment: m_comments) {
    m_objects_writer->addComment(comment);
  }
  m_comments.clear();
}

void LdacWriter::append(const Table& table) {
  assert(m_objects_writer);
  m_objects_writer->addData(table);
}

} // end of namespace SourceXtractor
