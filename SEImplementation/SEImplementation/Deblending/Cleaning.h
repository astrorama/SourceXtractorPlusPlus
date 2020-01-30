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
 * Cleaning.h
 *
 *  Created on: 2018 M12 18
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_DEBLENDING_CLEANING_H_
#define _SEIMPLEMENTATION_DEBLENDING_CLEANING_H_

#include "SEUtils/Types.h"
#include "SEFramework/Pipeline/Deblending.h"
#include "SEFramework/Source/SourceInterface.h"
#include "SEFramework/Source/SourceGroupInterface.h"
#include "SEFramework/Source/SourceFactory.h"

namespace ModelFitting {
  class ExtendedModel;
}

namespace SourceXtractor {

class MoffatModelFitting;

class Cleaning : public DeblendStep {

public:
  Cleaning(std::shared_ptr<SourceFactory> source_factory, unsigned int min_area) :
    m_source_factory(source_factory), m_min_area(min_area) {}
  virtual ~Cleaning() {}

  void deblend(SourceGroupInterface& group) const override;

private:
  bool shouldClean(SourceInterface& source, SourceGroupInterface& group) const;
  SourceGroupInterface::iterator findMostInfluentialSource(
      SourceInterface& source, const std::vector<SourceGroupInterface::iterator>& candidates) const;

  std::shared_ptr<SourceInterface> mergeSources(SourceInterface& parent,
      const std::vector<SourceGroupInterface::iterator> children) const;

  std::shared_ptr<SourceFactory> m_source_factory;
  unsigned int m_min_area;
};

}


#endif /* _SEIMPLEMENTATION_DEBLENDING_CLEANING_H_ */
