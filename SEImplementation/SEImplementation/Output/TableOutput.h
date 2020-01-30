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
  
  size_t flush() override {
    if (!m_rows.empty()) {
      Euclid::Table::Table table {m_rows};
      m_table_handler(table);
    }
    m_total_rows_written += m_rows.size();
    m_rows.clear();
    return m_total_rows_written;
  }
  
  TableOutput(SourceToRowConverter source_to_row, TableHandler table_handler, size_t flush_size)
          : m_source_to_row(source_to_row), m_table_handler(table_handler),
            m_flush_size(flush_size), m_total_rows_written(0) {
  }

  void outputSource(const SourceInterface& source) override {
    m_rows.emplace_back(m_source_to_row(source));
    if (m_flush_size > 0 && m_rows.size() % m_flush_size == 0) {
      flush();
    }
  }
  
private:
  SourceToRowConverter m_source_to_row;
  TableHandler m_table_handler;
  std::vector<Euclid::Table::Row> m_rows {};
  size_t m_flush_size;
  size_t m_total_rows_written;
};

} /* namespace SourceXtractor */

#endif /* _SEIMPLEMENTATION_TABLEOUTPUT_H */

