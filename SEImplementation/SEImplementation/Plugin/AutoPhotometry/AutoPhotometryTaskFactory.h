/*
 * AutoPhotometryTaskFactory.h
 *
 *  Created on: Jul 18, 2018
 *      Author: mkuemmel@usm.lmu.de
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_AUTOPHOTOMETRY_AUTOPHOTOMETRYTASKFACTORY_H_
#define _SEIMPLEMENTATION_PLUGIN_AUTOPHOTOMETRY_AUTOPHOTOMETRYTASKFACTORY_H_

#include "SEFramework/Task/TaskFactory.h"

// TEMP
#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/VectorImage.h"
#include "SEFramework/Image/SubtractImage.h"
#include "SEFramework/Image/WriteableImage.h"
//#include "SEFramework/Image/FitsWriter.h"

namespace SExtractor {

class AutoPhotometryTaskFactory : public TaskFactory {
public:
  AutoPhotometryTaskFactory() : m_magnitude_zero_point(0) {}

  /// Destructor
  virtual ~AutoPhotometryTaskFactory() = default;
  //virtual ~AutoPhotometryTaskFactory(){
  //  std::string bbb("tmp.fits");
  //  FitsWriter::writeFile(*m_tmp_check_image, bbb);
  //}

  void reportConfigDependencies(Euclid::Configuration::ConfigManager& manager) const override;

  void configure(Euclid::Configuration::ConfigManager& manager) override;

  // TaskFactory implementation
  virtual std::shared_ptr<Task> createTask(const PropertyId& property_id) const override;

private:
  SeFloat m_magnitude_zero_point;
  SeFloat m_kron_factor;
  SeFloat m_kron_minrad;
  SeFloat m_kron_estimation;
  SeFloat m_kron_measurement;

  // TEMP
  std::shared_ptr<WriteableImage<float>> m_tmp_check_image;

};

}

#endif /* _SEIMPLEMENTATION_PLUGIN_AUTOPHOTOMETRY_AUTOPHOTOMETRYTASKFACTORY_H_ */
