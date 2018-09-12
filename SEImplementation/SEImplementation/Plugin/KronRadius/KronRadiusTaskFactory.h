/*
 * KronRadiusTaskFactory.h
 *
 *  Created on: Sep 18, 2018
 *      Author: mkuemmel@usm.lmu.de
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_KRONRADIUS_KRONRADIUSTASKFACTORY_H_
#define _SEIMPLEMENTATION_PLUGIN_KRONRADIUS_KRONRADIUSTASKFACTORY_H_

#include "SEFramework/Task/TaskFactory.h"

namespace SExtractor {

class KronRadiusTaskFactory : public TaskFactory {
public:
  KronRadiusTaskFactory() {};

  /// Destructor
  virtual ~KronRadiusTaskFactory() = default;

  //void configure(Euclid::Configuration::ConfigManager& manager) override;

  // TaskFactory implementation
  virtual std::shared_ptr<Task> createTask(const PropertyId& property_id) const override;
/*
private:
  SeFloat m_magnitude_zero_point;
  SeFloat m_kron_factor;
  SeFloat m_kron_minrad;
  SeFloat m_kron_estimation;
  SeFloat m_kron_measurement;

  // TEMP
  std::shared_ptr<WriteableImage<float>> m_tmp_check_image;
*/
};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_KRONRADIUS_KRONRADIUSTASKFACTORY_H_ */
