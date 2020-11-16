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
#include "SEFramework/Pipeline/Prefetcher.h"

static Elements::Logging logger = Elements::Logging::getLogger("Prefetcher");


namespace SourceXtractor {

Prefetcher::Prefetcher(const std::shared_ptr<Euclid::ThreadPool>& thread_pool)
  : m_thread_pool(thread_pool) {
  m_output_thread = Euclid::make_unique<std::thread>(&Prefetcher::outputLoop, this);
}

Prefetcher::~Prefetcher() {
  m_stop = true;
  m_output_thread->join();
}

void Prefetcher::handleMessage(const std::shared_ptr<SourceInterface>& message) {
  // Shortcut for when there are no prefetch properties
  if (m_prefetch_set.empty()) {
    notifyObservers(message);
    return;
  }

  // Pre-fetch in separate threads
  m_thread_pool->submit([this, message]() {
    for (auto& prop : m_prefetch_set) {
      message->getProperty(prop);
    }
    std::unique_lock<std::mutex> lock(m_output_queue_mutex);
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
  while (true) {
    std::unique_lock<std::mutex> output_lock(m_output_queue_mutex);

    // Wait for something in the output queue
    if (m_output_queue.empty()) {
      m_new_output.wait_for(output_lock, std::chrono::milliseconds(100));
    }

    // Process the output queue
    while (!m_output_queue.empty()) {
      notifyObservers(m_output_queue.front());
      m_output_queue.pop_front();
    }

    if (m_stop && m_output_queue.empty()) {
      break;
    }
  }
  logger.debug() << "Stopping prefetcher output loop";
}

} // end of namespace SourceXtractor
