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
 * @file SimpleSourceGroupFactory.h
 * @author nikoapos
 */

#ifndef _SEFRAMEWORK_SIMPLESOURCEGROUPFACTORY_H
#define _SEFRAMEWORK_SIMPLESOURCEGROUPFACTORY_H

#include "SEFramework/Source/SourceGroupFactory.h"
#include "SEFramework/Source/SimpleSourceGroup.h"

namespace SourceXtractor {

/**
 * @class SimpleSourceGroupFactory
 * @brief A SourceGroupFactory implementation that produces a SimpleSourceGroupFactory
 */

class SimpleSourceGroupFactory : public SourceGroupFactory {

public:

  std::unique_ptr<SourceGroupInterface> createSourceGroup() const override {
    return std::make_unique<SimpleSourceGroup>();
  }

};

}

#endif /* _SEFRAMEWORK_SIMPLESOURCEGROUPFACTORY_H */

