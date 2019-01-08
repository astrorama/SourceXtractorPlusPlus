/* 
 * @file TableOutput.h
 * @author nikoapos
 */

#ifndef _SEIMPLEMENTATION_TABLEOUTPUT_H
#define _SEIMPLEMENTATION_TABLEOUTPUT_H

#include "Table/Table.h"
#include "Table/CastVisitor.h"

#include "SEFramework/Output/Output.h"

namespace SExtractor {

class TableOutput : public Output {
  
public:
  
  using SourceToRowConverter = std::function<Euclid::Table::Row(const SourceInterface&)>;
  using TableHandler = std::function<void(const Euclid::Table::Table&)>;
  
  void flush() override {
    if (!m_rows.empty()) {
      Euclid::Table::Table table {m_rows};
      m_table_handler(table);
      std::cout << m_rows.size() << " sources detected\n";
    } else {
      std::cout << "NO SOURCES DETECTED\n";
    }
  }
  
  TableOutput(SourceToRowConverter source_to_row, TableHandler table_handler) 
          : m_source_to_row(source_to_row), m_table_handler(table_handler) {
  }

  void outputSource(const SourceInterface& source) override {
    m_rows.emplace_back(m_source_to_row(source));
  }
  
private:
  SourceToRowConverter m_source_to_row;
  TableHandler m_table_handler;
  std::vector<Euclid::Table::Row> m_rows {};
  
};

} /* namespace SExtractor */

#endif /* _SEIMPLEMENTATION_TABLEOUTPUT_H */

