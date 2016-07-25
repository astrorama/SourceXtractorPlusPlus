/* 
 * @file TableOutput.h
 * @author nikoapos
 */

#ifndef _SEIMPLEMENTATION_TABLEOUTPUT_H
#define _SEIMPLEMENTATION_TABLEOUTPUT_H

#include "Table/Table.h"
#include "Table/CastVisitor.h"

#include "SEFramework/Output/Output.h"
#include "SEFramework/Output/OutputColumn.h"

namespace SExtractor {

class TableOutput : public Output {
  
public:
  
  using TableHandler = std::function<void(const Euclid::Table::Table&)>;
  
  virtual ~TableOutput() {
    Euclid::Table::Table table {m_rows};
    m_table_handler(table);
  }
  
  TableOutput(std::vector<OutputColumn> output_columns, TableHandler table_handler) 
          : m_output_columns(output_columns), m_table_handler(table_handler) {
    std::vector<Euclid::Table::ColumnInfo::info_type> info_list = {};
    for (auto& oc : m_output_columns) {
      info_list.emplace_back(oc.getColumnName(), typeid(std::string));
    }
    m_column_info = std::make_shared<Euclid::Table::ColumnInfo>(std::move(info_list));
  }

  void outputSource(const SourceInterface& source) override {
    std::vector<Euclid::Table::Row::cell_type> cells {};
    for (auto& oc : m_output_columns) {
      auto cell_value = oc.getValue(source);
      auto cell_value_str = boost::apply_visitor(Euclid::Table::CastVisitor<std::string>{}, cell_value);
      cells.emplace_back(cell_value_str);
    }
    m_rows.emplace_back(std::move(cells), m_column_info);
  }
  
private:
  
  std::vector<OutputColumn> m_output_columns;
  TableHandler m_table_handler;
  std::shared_ptr<Euclid::Table::ColumnInfo> m_column_info {};
  std::vector<Euclid::Table::Row> m_rows {};
  
};

} /* namespace SExtractor */

#endif /* _SEIMPLEMENTATION_TABLEOUTPUT_H */

