/*
 * MultiThreadedMeasurement.cpp
 *
 *  Created on: May 23, 2018
 *      Author: mschefer
 */


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
    notifyObservers(source_group);
  }
}


void MultithreadedMeasurement::handleMessage(const std::shared_ptr<SourceGroupInterface>& source_group) {
  m_input_queue_mutex.lock();
  m_input_queue.emplace_back(source_group);
  m_input_queue_mutex.unlock();
}

void MultithreadedMeasurement::threadStatic(MultithreadedMeasurement* measurement) {
  measurement->threadLoop();
}

void MultithreadedMeasurement::threadLoop() {
  while(true) {
    std::shared_ptr<SourceGroupInterface> source_group;

    m_input_queue_mutex.lock();
    if (m_input_queue.empty()) {
      m_input_queue_mutex.unlock();
      break;
    }

    source_group = m_input_queue.front();
    m_input_queue.pop_front();
    m_input_queue_mutex.unlock();

    //g_global_mutex.lock();
    for (auto& source : *source_group) {
      m_source_to_row(source);
    }
    //g_global_mutex.unlock();

    m_output_queue_mutex.lock();
    m_output_queue.emplace_back(source_group);
    m_output_queue_mutex.unlock();
  }
}

}

