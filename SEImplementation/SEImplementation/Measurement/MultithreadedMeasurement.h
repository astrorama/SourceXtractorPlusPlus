/*
 * Multithreadedmeasurement->h
 *
 *  Created on: May 17, 2018
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_OUTPUT_MULTITHREADEDMEASUREMENT_H_
#define _SEIMPLEMENTATION_OUTPUT_MULTITHREADEDMEASUREMENT_H_

#include <thread>
#include <mutex>

#include "SEFramework/Pipeline/Measurement.h"

namespace SExtractor {

class MultithreadedMeasurement : public Measurement {
public:

  using SourceToRowConverter = std::function<Euclid::Table::Row(const SourceInterface&)>;
  MultithreadedMeasurement(SourceToRowConverter source_to_row, int worker_threads_nb)
      : m_source_to_row(source_to_row), m_worker_threads_nb(worker_threads_nb), m_group_counter(0) {}

  virtual void performMeasurements();
  virtual void handleMessage(const std::shared_ptr<SourceGroupInterface>& source_group) override;

public:
  static std::recursive_mutex g_global_mutex;

private:
  static void threadStatic(MultithreadedMeasurement* measurement);
  void threadLoop();

  SourceToRowConverter m_source_to_row;

  int m_worker_threads_nb;
  std::vector<std::shared_ptr<std::thread>> m_worker_threads;

  int m_group_counter;
  std::list<std::pair<int, std::shared_ptr<SourceGroupInterface>>> m_input_queue;
  std::mutex m_input_queue_mutex;

  std::map<int, std::shared_ptr<SourceGroupInterface>> m_output_queue;
  std::mutex m_output_queue_mutex;
};

}

#endif /* _SEIMPLEMENTATION_OUTPUT_MULTITHREADEDMEASUREMENT_H_ */
