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
 * Multithreadedmeasurement->h
 *
 *  Created on: May 17, 2018
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_OUTPUT_MULTITHREADEDMEASUREMENT_H_
#define _SEIMPLEMENTATION_OUTPUT_MULTITHREADEDMEASUREMENT_H_

#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include "SEFramework/Pipeline/Measurement.h"

namespace SExtractor {

class MultithreadedMeasurement : public Measurement {
public:

  using SourceToRowConverter = std::function<Euclid::Table::Row(const SourceInterface&)>;
  MultithreadedMeasurement(SourceToRowConverter source_to_row, int worker_threads_nb)
      : m_source_to_row(source_to_row),
        m_worker_threads_nb(worker_threads_nb),
        m_active_threads(0),
        m_group_counter(0),
        m_input_done(false), m_abort_raised(false) {}

  virtual void handleMessage(const std::shared_ptr<SourceGroupInterface>& source_group) override;

  virtual void startThreads();
  virtual void waitForThreads();

public:
  static std::recursive_mutex g_global_mutex;

private:
  static void workerThreadStatic(MultithreadedMeasurement* measurement, int id);
  static void outputThreadStatic(MultithreadedMeasurement* measurement, int id);
  void workerThreadLoop();
  void outputThreadLoop();

  SourceToRowConverter m_source_to_row;

  std::shared_ptr<std::thread> m_output_thread;

  int m_worker_threads_nb;
  std::vector<std::shared_ptr<std::thread>> m_worker_threads;

  int m_active_threads;
  std::mutex m_active_threads_mutex;

  int m_group_counter;
  std::atomic_bool m_input_done, m_abort_raised;
  std::condition_variable m_new_input;
  std::list<std::pair<int, std::shared_ptr<SourceGroupInterface>>> m_input_queue;
  std::mutex m_input_queue_mutex;

  std::condition_variable m_new_output;
  std::list<std::pair<int, std::shared_ptr<SourceGroupInterface>>> m_output_queue;
  std::mutex m_output_queue_mutex;
};

}

#endif /* _SEIMPLEMENTATION_OUTPUT_MULTITHREADEDMEASUREMENT_H_ */
