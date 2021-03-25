/*
 * AssocModeProperty.h
 *
 *  Created on: Mar 5, 2021
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_ASSOCMODE_ASSOCMODE_H_
#define _SEIMPLEMENTATION_PLUGIN_ASSOCMODE_ASSOCMODE_H_

#include "NdArray/NdArray.h"

#include "SEUtils/Types.h"
#include "SEFramework/Property/Property.h"
#include "SEFramework/Source/SourceFlags.h"

namespace SourceXtractor {

/**
 * @class AssocMode
 * @brief
 */
class AssocMode : public Property {
public:
  template<typename T>
  using NdArray = Euclid::NdArray::NdArray<T>;

  /**
   * @brief Destructor
   */
  virtual ~AssocMode() = default;

  AssocMode(bool has_assoc, const std::vector<double>& assoc_data) :
    m_has_assoc(has_assoc), m_assoc_data(std::vector<size_t>({assoc_data.size()})) {
    for (size_t i=0; i<assoc_data.size(); i++) {
      m_assoc_data.at(i) = assoc_data[i];
    }
  }

  bool getMatch() const {
    return m_has_assoc;
  }

  const NdArray<SeFloat>& getAssocValues() const {
    return m_assoc_data;
  }

private:
  bool m_has_assoc;

  NdArray<SeFloat> m_assoc_data;
};

} /* namespace SourceXtractor */

#endif /* _SEIMPLEMENTATION_PLUGIN_ASSOCMODE_ASSOCMODE_H_ */
