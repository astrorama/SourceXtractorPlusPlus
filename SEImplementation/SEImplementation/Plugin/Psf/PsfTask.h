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
 * PsfTask.h
 *
 *  Created on: Jun 25, 2018
 *      Author: Alejandro Álvarez Ayllón
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_PSF_PSFTASK_H_
#define _SEIMPLEMENTATION_PLUGIN_PSF_PSFTASK_H_

#include "SEFramework/Task/GroupTask.h"
#include "SEFramework/Psf/Psf.h"

namespace SourceXtractor {

class PsfTask: public GroupTask {
public:
  virtual ~PsfTask() = default;

  PsfTask(unsigned instance, const std::shared_ptr<Psf> &vpsf);

  void computeProperties(SourceGroupInterface& source) const override;

  typedef std::function<double(SourceXtractor::SourceGroupInterface &group, unsigned instance)> ValueGetter;
  static std::map<std::string, ValueGetter> component_value_getters;

private:
  unsigned m_instance;
  std::shared_ptr<Psf> m_vpsf;
};

} // end SourceXtractor

#endif //_SEIMPLEMENTATION_PLUGIN_PSF_PSFTASK_H_
