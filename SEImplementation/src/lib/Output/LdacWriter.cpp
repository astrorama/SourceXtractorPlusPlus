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

#include <sstream>
#include <AlexandriaKernel/memory_tools.h>
#include "SEFramework/Property/DetectionFrame.h"
#include "SEImplementation/Output/LdacWriter.h"
#include "SEImplementation/Configuration/DetectionImageConfig.h"
#include "SOURCEXTRACTORPLUSPLUS_VERSION.h"

#if BOOST_VERSION < 107300
#include <boost/io/detail/quoted_manip.hpp>
#else
#include <boost/io/quoted.hpp>
#endif

namespace SourceXtractor {

using Euclid::Configuration::ConfigManager;
using Euclid::make_unique;
using namespace Euclid::Table;


LdacWriter::LdacWriter(const std::string& filename, ConfigManager& manager)
  : m_config_manager(manager), m_filename(filename), m_rms(0) {
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
std::string generateHeader(const std::string& name, T value, const std::string& comment) {
  std::stringstream str;

  str << std::setw(8) << std::left << name << "= "
      << std::scientific << std::setw(20) << std::right << value
      << " / ";

  size_t remaining = 80 - str.str().size();
  str << comment << std::string(remaining-comment.size(), ' ');

  if (str.str().size() != 80) {
    throw Elements::Exception() << "Header must be exactly 80 characters long: \"" << str.str() << "\"";
  }

  return str.str();
}

template<>
std::string generateHeader<std::string>(const std::string& name, std::string value, const std::string& comment) {
  std::stringstream str, quoted_value;

  quoted_value << boost::io::quoted(value, '\'', '\'');

  str << std::setw(8) << std::left << name << "= "
      << std::setw(20) << std::left << quoted_value.str()
      << " / ";

  size_t remaining = 80 - str.str().size();
  str << comment << std::string(remaining-comment.size(), ' ');

  if (str.str().size() != 80) {
    throw Elements::Exception() << "Header must be exactly 80 characters long: \"" << str.str() << "\"";
  }

  return str.str();
}

static void generateHistory(std::vector<std::string>& headers) {
  std::vector<std::string> entries;
  std::stringstream str;

  str << "Version " << SOURCEXTRACTORPLUSPLUS_VERSION_STRING;
  entries.emplace_back(str.str());
  str.str("");

  auto t = std::time(nullptr);
  auto now = *std::gmtime(&t);

  char date_str[32];
  strftime(date_str, sizeof(date_str), "%Y-%m-%dT%H:%M:%SZ", &now);
  str << "Called at " << date_str;
  entries.emplace_back(str.str());
  str.str("");

  for (auto& e : entries) {
    std::stringstream padder;
    padder << "HISTORY s++: " << std::setw(67) << std::left << e;
    headers.emplace_back(padder.str());
  }
}

void LdacWriter::writeImHead() {
  auto& detection_image_config = m_config_manager.getConfiguration<DetectionImageConfig>();
  auto imhead_writer = Euclid::make_unique<FitsWriter>(m_filename, true);
  imhead_writer->setHduName("LDAC_IMHEAD");

  // Headers from the image
  std::vector<std::string> ldac_imhead;
  auto img_source = detection_image_config.getImageSource();
  auto img_metadata = img_source->getMetadata();
  for (const auto &p : img_metadata) {
    std::string comment;
    if (p.second.m_extra.count("comment"))
      comment = p.second.m_extra.at("comment");
    if (p.second.m_value.type() == typeid(std::string))
      ldac_imhead.emplace_back(generateHeader(p.first, boost::get<std::string>(p.second.m_value), comment));
    else
      ldac_imhead.emplace_back(generateHeader(p.first, p.second.m_value, comment));
  }

  // Headers from the configuration and detection
  auto gain = detection_image_config.getGain();
  ldac_imhead.emplace_back(generateHeader("SPPGAIN", gain, "Gain used"));
  ldac_imhead.emplace_back(generateHeader("SPPBKDEV", m_rms, "Median background RMS"));

  // History, why not
  generateHistory(ldac_imhead);

  // END
  ldac_imhead.emplace_back("END" + std::string(77, ' '));

  // Write the table
  auto column_info = std::make_shared<ColumnInfo>(std::vector<ColumnInfo::info_type>{
    {"Field Header Card", typeid(std::string)}
  });
  std::vector<Row> rows;
  for (const auto& header : ldac_imhead) {
    rows.emplace_back(std::vector<Row::cell_type>{header}, column_info);
  }
  imhead_writer->addData(Table{std::vector<Row>{rows}});
}

void LdacWriter::init(const Table&) {
  // Initialize LDAC_IMHEAD HDU
  writeImHead();

  // Initialize LDAC_OBJECTS HDU
  m_objects_writer = Euclid::make_unique<FitsWriter>(m_filename, false);
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
