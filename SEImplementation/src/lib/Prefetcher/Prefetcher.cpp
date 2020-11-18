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

#include <ElementsKernel/Logging.h>
#include "AlexandriaKernel/memory_tools.h"
#include "SEImplementation/Property/SourceId.h"
#include "SEImplementation/Prefetcher/Prefetcher.h"

static Elements::Logging logger = Elements::Logging::getLogger("Prefetcher");


namespace SourceXtractor {

Prefetcher::Prefetcher(const std::shared_ptr<Euclid::ThreadPool>& thread_pool)
  : m_thread_pool(thread_pool), m_last_received(-1), m_stop(false) {
  m_output_thread = Euclid::make_unique<std::thread>(&Prefetcher::outputLoop, this);
}

Prefetcher::~Prefetcher() {
  if (m_output_thread->joinable())
    wait();
}

void Prefetcher::handleMessage(const std::shared_ptr<SourceInterface>& message) {
  // Remember this source
  // They are assumed to come in order from the partion stage
  auto source_id = message->getProperty<SourceId>().getSourceId();
  assert(source_id > m_last_received);
  std::lock_guard<std::mutex> ongoing_lock(m_ongoing_mutex);
  m_last_received = source_id;
  m_ongoing.emplace(m_last_received);

  // Pre-fetch in separate threads
  m_thread_pool->submit([this, message]() {
    for (auto& prop : m_prefetch_set) {
      message->getProperty(prop);
    }
    std::lock_guard<std::mutex> lock(m_output_queue_mutex);
    m_output_queue.emplace_back(message);
    m_new_output.notify_one();
  });
}

void Prefetcher::requestProperty(const PropertyId& property_id) {
  m_prefetch_set.emplace(property_id);
  logger.debug() << "Requesting prefetch of " << property_id.getString();
}

void Prefetcher::outputLoop() {
  logger.debug() << "Starting prefetcher output loop";

  bool event_sent = false;
  while (true) {
    std::unique_lock<std::mutex> output_lock(m_output_queue_mutex);

    // Wait for something in the output queue
    // If there are things on the queue, but there are events too, also wait
    // unless on the last iteration one, or more, events were sent downstream
    if (m_output_queue.empty() || (!m_event_queue.empty() && !event_sent)) {
      m_new_output.wait_for(output_lock, std::chrono::milliseconds(1000));
    }

    // Need to access the ongoing id set now
    std::lock_guard<std::mutex> ongoing_lock(m_ongoing_mutex);

    // Process the output queue
    for (auto qi = m_output_queue.begin(); qi != m_output_queue.end();) {
      auto source = *qi;
      auto source_id = source->getProperty<SourceId>().getSourceId();

      // If there is an event on the queue, do not pass any source that came after
      if (!m_event_queue.empty() && m_event_queue.front().first < source_id) {
        logger.debug() << "ProcessSourceEvent " << m_event_queue.front().first
                       << " queued, holding " << source_id;
        ++qi;
        continue;
      }
      logger.debug() << "Source " << source_id << " sent downstream";
      qi = m_output_queue.erase(qi);
      Observable<std::shared_ptr<SourceInterface>>::notifyObservers(source);
      m_ongoing.erase(source_id);
    }

    // If there is an event on the queue, pass it iff every source received before
    // has been already finished
    event_sent = false;
    while (!m_event_queue.empty()) {
      auto event = m_event_queue.front();

      // Verify there are no sources prior to the moment the event was received
      auto prior_i = std::find_if(m_ongoing.begin(), m_ongoing.end(),
                                  [event](unsigned id) { return id <= event.first; }
      );
      if (prior_i == m_ongoing.end()) {
        m_event_queue.pop_front();
        logger.debug() << "ProcessSourceEvent " << event.first << " released";
        Observable<ProcessSourcesEvent>::notifyObservers(event.second);
        event_sent = true;
      }
      else {
        break;
      }
    }

    if (m_stop && m_output_queue.empty() && m_ongoing.empty()) {
      break;
    }
  }
  logger.debug() << "Stopping prefetcher output loop";
}

void Prefetcher::handleMessage(const ProcessSourcesEvent& message) {
  std::lock_guard<std::mutex> output_lock(m_output_queue_mutex);
  m_event_queue.emplace_back(m_last_received, message);
  m_new_output.notify_one();
  logger.debug() << "ProcessSourceEvent received after " << m_last_received;
}

void Prefetcher::wait() {
  m_stop = true;
  m_output_thread->join();
}

} // end of namespace SourceXtractor
