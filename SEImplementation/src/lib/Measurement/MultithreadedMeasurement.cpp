/*
 * MultiThreadedMeasurement.cpp
 *
 *  Created on: May 23, 2018
 *      Author: mschefer
 */

#include "SEImplementation/Plugin/SourceIDs/SourceID.h"

#include "SEImplementation/Measurement/MultithreadedMeasurement.h"

namespace SExtractor {

std::recursive_mutex MultithreadedMeasurement::g_global_mutex;

void MultithreadedMeasurement::performMeasurements() {
  for (int i=0; i<m_worker_threads_nb; i++) {
    m_worker_threads.emplace_back(std::make_shared<std::thread>(threadStatic, this));
  }

  // Wait for all threads to finish
  for (int i=0; i<m_worker_threads_nb; i++) {
    m_worker_threads[i]->join();
  }

  for (auto source_group : m_output_queue) {
    notifyObservers(source_group.second);
  }
}


void MultithreadedMeasurement::handleMessage(const std::shared_ptr<SourceGroupInterface>& source_group) {
  m_input_queue_mutex.lock();
  for (auto& source : *source_group) {
    source.getProperty<SourceID>();
  }
  m_input_queue.emplace_back(m_group_counter++, source_group);
  m_input_queue_mutex.unlock();
}

void MultithreadedMeasurement::threadStatic(MultithreadedMeasurement* measurement) {
  measurement->threadLoop();
}

void MultithreadedMeasurement::threadLoop() {
  while(true) {
    m_input_queue_mutex.lock();
    if (m_input_queue.empty()) {
      m_input_queue_mutex.unlock();
      break;
    }

    auto order_number = m_input_queue.front().first;
    auto source_group = m_input_queue.front().second;
    m_input_queue.pop_front();

    m_input_queue_mutex.unlock();

    for (auto& source : *source_group) {
      m_source_to_row(source);
    }

    m_output_queue_mutex.lock();
    m_output_queue[order_number] = source_group;
    m_output_queue_mutex.unlock();
  }
}

}

