/*
 * OutputColumn.h
 *
 *  Created on: Jun 23, 2016
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_OUTPUT_OUTPUTCOLUMN_H_
#define _SEFRAMEWORK_OUTPUT_OUTPUTCOLUMN_H_

#include "Table/Row.h"
#include <boost/any.hpp>
#include "SEFramework/Property/PropertyId.h"
#include "SEFramework/Source/SourceInterface.h"

namespace SExtractor {

class OutputColumn {
  
public:
  
//  using cell_type = Euclid::Table::Row::cell_type;
  using cell_type = boost::any;

  template<typename PropertyType>
  using GetterFunction = std::function<cell_type (const PropertyType&)>;

  virtual ~OutputColumn() = default;

  template<typename PropertyType>
  OutputColumn(std::string column_name, GetterFunction<PropertyType> getter, std::size_t index=0) :
      m_column_name(column_name),
      m_getter([getter, index](const SourceInterface& source) {
        auto& property = source.getProperty<PropertyType>(index);
        return getter(property);
      }) {
  }

  cell_type getValue(const SourceInterface& source) const {
    return m_getter(source);
  }

  const std::string& getColumnName() const {
    return m_column_name;
  }

protected:
  std::string m_column_name;
  std::function<cell_type (const SourceInterface&)> m_getter;
};


} // namespace SExtractor



#endif /* _SEFRAMEWORK_OUTPUT_OUTPUTCOLUMN_H_ */
