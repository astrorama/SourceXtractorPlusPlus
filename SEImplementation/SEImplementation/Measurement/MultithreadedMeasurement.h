/*
 * Multithreadedmeasurement->h
 *
 *  Created on: May 17, 2018
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_OUTPUT_MULTITHREADEDMEASUREMENT_H_
#define _SEIMPLEMENTATION_OUTPUT_MULTITHREADEDMEASUREMENT_H_

#include <boost/thread.hpp>

#include "SEFramework/Pipeline/Measurement.h"

namespace SExtractor {

class MultithreadedMeasurement : public Measurement {
public:

  MultithreadedMeasurement(int worker_threads_nb) : m_worker_threads_nb(worker_threads_nb) {}

  virtual void performMeasurements() {
    for (int i=0; i<m_worker_threads_nb; i++) {
      m_worker_threads.emplace_back(threadStatic, this);
    }

    // Wait for all threads to finish
    for (int i=0; i<m_worker_threads_nb; i++) {
      m_worker_threads[i].join();
    }

    for (auto source_group : m_output_queue) {
      notifyObservers(source_group);
    }
  }


  virtual void handleMessage(const std::shared_ptr<SourceGroupInterface>& source_group) override {
    m_input_queue_mutex.lock();
    m_input_queue.emplace_back(source_group);
    m_input_queue_mutex.unlock();
  }

private:
  static void threadStatic(MultithreadedMeasurement* measurement) {
    measurement->threadLoop();
  }

  void threadLoop() {

    while(true) {
      std::shared_ptr<SourceGroupInterface> group;

      m_input_queue_mutex.lock();
      if (m_input_queue.empty()) {
        m_input_queue_mutex.unlock();
        break;
      }

      group = m_input_queue.front();
      m_input_queue.pop_front();
      m_input_queue_mutex.unlock();

      // TODO do the measurements here

      m_output_queue_mutex.lock();
      m_output_queue.emplace_back(group);
      m_output_queue_mutex.unlock();
    }
  }

  int m_worker_threads_nb;
  std::vector<boost::thread> m_worker_threads;

  std::list<std::shared_ptr<SourceGroupInterface>> m_input_queue;
  boost::mutex m_input_queue_mutex;

  std::list<std::shared_ptr<SourceGroupInterface>> m_output_queue;
  boost::mutex m_output_queue_mutex;
};

}

#endif /* _SEIMPLEMENTATION_OUTPUT_MULTITHREADEDMEASUREMENT_H_ */
