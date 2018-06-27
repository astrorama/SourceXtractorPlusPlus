/*
 * PsfTask.h
 *
 *  Created on: Jun 25, 2018
 *      Author: Alejandro Álvarez Ayllón
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_PSF_PSFTASK_H_
#define _SEIMPLEMENTATION_PLUGIN_PSF_PSFTASK_H_

#include "SEFramework/Psf/VariablePsf.h"
#include "SEFramework/Task/SourceTask.h"

namespace SExtractor {

typedef std::function<double(SExtractor::SourceInterface &source)> ValueGetter;
extern std::map<std::string, ValueGetter> component_value_getters;

class PsfTask: public SourceTask {
public:
  virtual ~PsfTask() = default;

  PsfTask(const std::shared_ptr<VariablePsf> &vpsf);

  virtual void computeProperties(SourceInterface& source) const override;

private:
  std::shared_ptr<VariablePsf> m_vpsf;
};

} // end SExtractor

#endif //_SEIMPLEMENTATION_PLUGIN_PSF_PSFTASK_H_
