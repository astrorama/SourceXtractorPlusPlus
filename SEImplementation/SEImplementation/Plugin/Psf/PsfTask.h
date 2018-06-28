/*
 * PsfTask.h
 *
 *  Created on: Jun 25, 2018
 *      Author: Alejandro Álvarez Ayllón
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_PSF_PSFTASK_H_
#define _SEIMPLEMENTATION_PLUGIN_PSF_PSFTASK_H_

#include "SEFramework/Task/GroupTask.h"
#include "SEFramework/Psf/VariablePsf.h"

namespace SExtractor {

typedef std::function<double(SExtractor::SourceGroupInterface &group)> ValueGetter;
extern std::map<std::string, ValueGetter> component_value_getters;

class PsfTask: public GroupTask {
public:
  virtual ~PsfTask() = default;

  PsfTask(unsigned instance, const std::shared_ptr<VariablePsf> &vpsf);

  virtual void computeProperties(SourceGroupInterface& source) const override;

private:
  unsigned m_instance;
  std::shared_ptr<VariablePsf> m_vpsf;
};

} // end SExtractor

#endif //_SEIMPLEMENTATION_PLUGIN_PSF_PSFTASK_H_
