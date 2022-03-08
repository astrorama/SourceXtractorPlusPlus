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
#include "SEImplementation/Prefetcher/Prefetcher.h"

static Elements::Logging logger = Elements::Logging::getLogger("Prefetcher");


namespace SourceXtractor {

/**
 * Unlock/lock an existing lock using RAII
 */
template<typename Lock>
struct ReverseLock {
  explicit ReverseLock(Lock& lock) : m_lock(lock) {
    m_lock.unlock();
  }

  ~ReverseLock() {
    m_lock.lock();
  }

private:
  Lock& m_lock;
};

Prefetcher::Prefetcher(const std::shared_ptr<Euclid::ThreadPool>& thread_pool, unsigned max_queue_size)
  : m_thread_pool(thread_pool), m_stop(false), m_semaphore(max_queue_size) {
  m_output_thread = Euclid::make_unique<std::thread>(&Prefetcher::outputLoop, this);
}

Prefetcher::~Prefetcher() {
  if (m_output_thread->joinable())
    wait();
}

void Prefetcher::handleMessage(const std::shared_ptr<SourceInterface>& message) {
  m_semaphore.acquire();

  intptr_t source_addr = reinterpret_cast<intptr_t>(message.get());
  {
    std::lock_guard<std::mutex> queue_lock(m_queue_mutex);
    m_received.emplace_back(EventType::SOURCE, source_addr);
  }

  // Pre-fetch in separate threads
  m_thread_pool->submit([this, source_addr, message]() {
    for (auto& prop : m_prefetch_set) {
      message->getProperty(prop);
    }
    {
      std::lock_guard<std::mutex> lock(m_queue_mutex);
      m_finished_sources.emplace(source_addr, message);
    }
    m_new_output.notify_one();
  });
}

void Prefetcher::requestProperty(const PropertyId& property_id) {
  m_prefetch_set.emplace(property_id);
  logger.debug() << "Requesting prefetch of " << property_id.getString();
}

void Prefetcher::outputLoop() {
  logger.debug() << "Starting prefetcher output loop";

  while (m_thread_pool->activeThreads() > 0) {
    std::unique_lock<std::mutex> output_lock(m_queue_mutex);

    // Wait for something new
    m_new_output.wait_for(output_lock, std::chrono::milliseconds(1000));

    // Process the output queue
    // This is, release sources when the front of the received has been processed
    while (!m_received.empty()) {
      auto next = m_received.front();
      // If the front is a ProcessSourceEvent, everything received before is done,
      // so pass downstream
      if (next.m_event_type == EventType::PROCESS_SOURCE) {
        auto event = m_event_queue.front();
        m_event_queue.pop_front();
        logger.debug() << "ProcessSourceEvent released";
        {
          ReverseLock<decltype(output_lock)> release_lock(output_lock);
          Observable<ProcessSourcesEvent>::notifyObservers(event);
        }
        m_received.pop_front();
        continue;
      }
      // Find if the matching source is done
      auto processed = m_finished_sources.find(next.m_source_addr);
      // If not, we can't keep going, so exit here
      if (processed == m_finished_sources.end()) {
        logger.debug() << "Next source " << next.m_source_addr << " not done yet";
        break;
      }
      // If it is, send it downstream
      logger.debug() << "Source " << next.m_source_addr << " sent downstream";
      {
        ReverseLock<decltype(output_lock)> release_lock(output_lock);
        Observable<std::shared_ptr<SourceInterface>>::notifyObservers(processed->second);
      }
      m_finished_sources.erase(processed);
      m_received.pop_front();
      m_semaphore.release();
    }

    if (m_stop && m_received.empty()) {
      break;
    }
  }
  logger.debug() << "Stopping prefetcher output loop";
}

void Prefetcher::handleMessage(const ProcessSourcesEvent& message) {
  {
    std::lock_guard<std::mutex> output_lock(m_queue_mutex);
    m_received.emplace_back(EventType::PROCESS_SOURCE);
    m_event_queue.emplace_back(message);
  }
  m_new_output.notify_one();
  logger.debug() << "ProcessSourceEvent received";
}

void Prefetcher::wait() {
  m_stop = true;
  m_output_thread->join();
}

void Prefetcher::synchronize() {
  // Wait until the output queue is empty
  while (true) {
    {
      std::unique_lock<std::mutex> output_lock(m_queue_mutex);
      if (m_received.empty()) {
        break;
      }
      else if (m_thread_pool->checkForException(false)) {
        logger.fatal() << "An exception was thrown while computing a required property";
        m_thread_pool->checkForException(true);
      }
      else if (m_thread_pool->activeThreads() == 0) {
        throw Elements::Exception() << "No active threads and the queue is not empty! Please, report this as a bug";
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}


} // end of namespace SourceXtractor
