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
 * @file TableOutput.h
 * @author nikoapos
 */

#ifndef _SEIMPLEMENTATION_TABLEOUTPUT_H
#define _SEIMPLEMENTATION_TABLEOUTPUT_H

#include "Table/Table.h"
#include "Table/CastVisitor.h"

#include "SEFramework/Output/Output.h"

namespace SourceXtractor {

class TableOutput : public Output {
  
public:
  
  using SourceToRowConverter = std::function<Euclid::Table::Row(const SourceInterface&)>;
  using TableHandler = std::function<void(const Euclid::Table::Table&)>;
  using SourceHandler = std::function<void(const SourceInterface& source)>;
  
  size_t flush() override {
    if (!m_rows.empty()) {
      Euclid::Table::Table table {m_rows};
      m_table_writer->addData(table);
    }
    m_total_rows_written += m_rows.size();
    m_rows.clear();
    return m_total_rows_written;
  }
  
  void nextPart() override {
    m_part_nb++;
    auto fits_table_writer = std::dynamic_pointer_cast<Euclid::Table::FitsWriter>(m_table_writer);
    if (fits_table_writer != nullptr) {
      std::stringstream hdu_name;
      hdu_name << "CATALOG_" << m_part_nb;
      fits_table_writer->setHduName(hdu_name.str());
    }
  }

  TableOutput(SourceToRowConverter source_to_row, std::shared_ptr<Euclid::Table::TableWriter> table_writer,
      SourceHandler source_handler, size_t flush_size)
    : m_source_to_row(source_to_row), m_table_writer(table_writer), m_source_handler(source_handler),
      m_flush_size(flush_size), m_total_rows_written(0), m_part_nb(0) {
  }

  void outputSource(const SourceInterface& source) override {
    if (m_source_handler) {
      m_source_handler(source);
    }
    m_rows.emplace_back(m_source_to_row(source));
    if (m_flush_size > 0 && m_rows.size() % m_flush_size == 0) {
      flush();
    }
  }
  
private:

  SourceToRowConverter m_source_to_row;
  std::shared_ptr<Euclid::Table::TableWriter> m_table_writer;
  SourceHandler m_source_handler;
  std::vector<Euclid::Table::Row> m_rows {};
  size_t m_flush_size;
  size_t m_total_rows_written;

  int m_part_nb;
};

} /* namespace SourceXtractor */

#endif /* _SEIMPLEMENTATION_TABLEOUTPUT_H */

