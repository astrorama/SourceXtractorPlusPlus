/*
 * OutputColumn.h
 *
 *  Created on: Jun 23, 2016
 *      Author: mschefer
 */

#ifndef _SEFRAMEWORK_OUTPUT_OUTPUTCOLUMN_H_
#define _SEFRAMEWORK_OUTPUT_OUTPUTCOLUMN_H_

#include <boost/any.hpp>
#include "SEFramework/Property/PropertyId.h"
#include "SEFramework/Property/Property.h"
#include "SEFramework/Source/SourceInterface.h"

namespace SExtractor {

class OutputColumn {
public:

  using GetterFunction = std::function<boost::any (const Property&)>;

  virtual ~OutputColumn() = default;

  OutputColumn(std::string column_name, PropertyId property_id, GetterFunction getter) :
      m_column_name(column_name), m_property_id(property_id), m_getter(getter) {
  }

  boost::any getValue(const SourceInterface& source) const {
    auto& property = source.getProperty(m_property_id);
    return m_getter(property);
  }

  const std::string& getColumnName() const {
    return m_column_name;
  }

protected:
  std::string m_column_name;
  PropertyId m_property_id;
  GetterFunction m_getter;
};


} // namespace SExtractor



#endif /* _SEFRAMEWORK_OUTPUT_OUTPUTCOLUMN_H_ */
