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
 * MeasurementFrameGroupRectangleTaskFactory.h
 *
 *  Created on: Sep 5, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAMEGROUPRECTANGLE_MEASUREMENTFRAMEGROUPRECTANGLETASKFACTORY_H_
#define _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAMEGROUPRECTANGLE_MEASUREMENTFRAMEGROUPRECTANGLETASKFACTORY_H_


#include "SEFramework/Task/TaskFactory.h"

namespace SExtractor {

class MeasurementFrameGroupRectangleTaskFactory : public TaskFactory {

public:

  virtual ~MeasurementFrameGroupRectangleTaskFactory() = default;

  virtual std::shared_ptr<Task> createTask(const PropertyId& property_id) const override;

};

} // end SExtractor


#endif // _SEIMPLEMENTATION_PLUGIN_MEASUREMENTFRAMEGROUPRECTANGLE_MEASUREMENTFRAMEGROUPRECTANGLETASKFACTORY_H_
