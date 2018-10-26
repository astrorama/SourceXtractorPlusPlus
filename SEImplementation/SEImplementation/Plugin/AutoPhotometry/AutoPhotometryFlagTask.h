/*
 * AutoPhotometryFlagTask.h
 *
 *  Created on: Oct 10, 2018
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_AUTOPHOTOMETRY_AUTOPHOTOMETRYFLAGTASK_H_
#define _SEIMPLEMENTATION_PLUGIN_AUTOPHOTOMETRY_AUTOPHOTOMETRYFLAGTASK_H_

#include "SEUtils/Types.h"
#include "SEFramework/Task/SourceTask.h"

namespace SExtractor {

class AutoPhotometryFlagTask: public SourceTask {
public:

  virtual ~AutoPhotometryFlagTask() = default;

  AutoPhotometryFlagTask(SeFloat kron_factor, SeFloat kron_minrad,
    const std::map<std::string, std::vector<unsigned int>> &instances_per_group)
  : m_kron_factor{kron_factor}, m_kron_minrad{kron_minrad}, m_instances_per_group{instances_per_group} {}

  virtual void computeProperties(SourceInterface& source) const override;

private:
  SeFloat m_kron_factor;
  SeFloat m_kron_minrad;
  std::map<std::string, std::vector<unsigned int>> m_instances_per_group;
};

} // end SExtractor

#endif // _SEIMPLEMENTATION_PLUGIN_AUTOPHOTOMETRY_AUTOPHOTOMETRYFLAGTASK_H_
