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
 * MultiThreadingConfig.h
 *
 *  Created on: May 22, 2018
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_CONFIGURATION_MULTITHREADINGCONFIG_H_
#define _SEIMPLEMENTATION_CONFIGURATION_MULTITHREADINGCONFIG_H_

#include "Configuration/Configuration.h"
#include "AlexandriaKernel/ThreadPool.h"

namespace SourceXtractor {

class MultiThreadingConfig : public Euclid::Configuration::Configuration {
public:
  explicit MultiThreadingConfig(long manager_id);

  virtual ~MultiThreadingConfig() = default;

  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  void initialize(const UserValues& args) override;

  int getThreadsNb() const {
    return m_threads_nb;
  }

  const std::shared_ptr<Euclid::ThreadPool>& getThreadPool() const {
    return m_thread_pool;
  }

  unsigned getMaxQueueSize() const {
    return m_max_queue_size;
  }

private:
  int m_threads_nb, m_max_queue_size;
  std::shared_ptr<Euclid::ThreadPool> m_thread_pool;
};


}


#endif /* _SEIMPLEMENTATION_CONFIGURATION_MULTITHREADINGCONFIG_H_ */
