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
 * MultiThreadedMeasurement.cpp
 *
 *  Created on: May 23, 2018
 *      Author: mschefer
 */

#include <chrono>
#include <ElementsKernel/Logging.h>
#include <csignal>

#include "SEImplementation/Plugin/SourceIDs/SourceID.h"
#include "SEImplementation/Measurement/MultithreadedMeasurement.h"

using namespace SourceXtractor;

static Elements::Logging logger = Elements::Logging::getLogger("Multithreading");
constexpr unsigned int MULTITHREAD_PRIORITY = 4;


MultithreadedMeasurement::~MultithreadedMeasurement() {
  if (m_output_thread->joinable()) {
    m_output_thread->join();
  }
}

void MultithreadedMeasurement::startThreads() {
  m_output_thread = Euclid::make_unique<std::thread>(outputThreadStatic, this);
}

void MultithreadedMeasurement::waitForThreads() {
  m_input_done = true;
  m_thread_pool->block();
  m_output_thread->join();
  logger.debug() << "All worker threads done!";
}

void
MultithreadedMeasurement::handleMessage(const std::shared_ptr<SourceGroupInterface>& source_group) {
  // Force computation of SourceID here, where the order is still deterministic
  for (auto& source : *source_group) {
    source.getProperty<SourceID>();
  }

  // Put the new SourceGroup into the input queue
  auto order_number = m_group_counter;
  m_thread_pool->submit([this, order_number, source_group]() {
    // Trigger measurements
    for (auto& source : *source_group) {
      m_source_to_row(source);
    }
    // Pass to the output thread
    {
      std::unique_lock<std::mutex> output_lock(m_output_queue_mutex);
      m_output_queue.emplace_back(order_number, source_group);
    }
    m_new_output.notify_one();
  }, MULTITHREAD_PRIORITY);
  ++m_group_counter;
}

void MultithreadedMeasurement::outputThreadStatic(MultithreadedMeasurement *measurement) {
  logger.debug() << "Starting output thread";
  try {
    measurement->outputThreadLoop();
  }
  catch (const Elements::Exception& e) {
    logger.fatal() << "Output thread got an exception!";
    logger.fatal() << e.what();
    if (!measurement->m_abort_raised.exchange(true)) {
      logger.fatal() << "Aborting the execution";
      ::raise(SIGTERM);
    }
  }
  logger.debug() << "Stopping output thread";
}

void MultithreadedMeasurement::outputThreadLoop() {
  while (m_thread_pool->activeThreads() > 0) {
    std::unique_lock<std::mutex> output_lock(m_output_queue_mutex);

    // Wait for something in the output queue
    if (m_output_queue.empty()) {
      m_new_output.wait_for(output_lock, std::chrono::milliseconds(100));
    }

    // Process the output queue
    while (!m_output_queue.empty()) {
      notifyObservers(m_output_queue.front().second);
      m_output_queue.pop_front();
    }

    if (m_input_done && m_thread_pool->running() + m_thread_pool->queued() == 0 &&
        m_output_queue.empty()) {
      break;
    }
  }
}
