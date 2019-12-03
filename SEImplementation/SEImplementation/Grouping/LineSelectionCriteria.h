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
 * LineSelectionCriteria.h
 *
 *  Created on: Nov 20, 2019
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_GROUPING_LINESELECTIONCRITERIA_H_
#define _SEIMPLEMENTATION_GROUPING_LINESELECTIONCRITERIA_H_

#include "SEFramework/Pipeline/SourceGrouping.h"

namespace SourceXtractor {

class LineSelectionCriteria : public SelectionCriteria {
public:

  LineSelectionCriteria(int line_number) : m_line_number(line_number) {
  }

  virtual bool mustBeProcessed(const SourceInterface& ) const override;

private:
  int m_line_number;
};

}


#endif /* _SEIMPLEMENTATION_GROUPING_LINESELECTIONCRITERIA_H_ */
