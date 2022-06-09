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

#ifndef _SEIMPLEMENTATION_MEASUREMENT_PREFETCHER_H_
#define _SEIMPLEMENTATION_MEASUREMENT_PREFETCHER_H_

#include <condition_variable>
#include "AlexandriaKernel/ThreadPool.h"
#include "AlexandriaKernel/Semaphore.h"
#include "SEFramework/Source/SourceInterface.h"
#include "SEFramework/Pipeline/PipelineStage.h"

namespace SourceXtractor {

/**
 * The pre-fetcher allows later stages, as the grouping or the cleaning,
 * to ask in advance for some compute intensive properties, so they can be done
 * multi-threaded before it reaches them.
 *
 * The pre-fetcher *must* handle also ProcessSourcesEvent, as they are synchronization points.
 * When one is received, only sources detected *before* the event will be passed along. Everyone
 * else will have to wait until there are no more soures prior to the event being processed.
 * Then, they will be released and sent along.
 *
 */
class Prefetcher : public PipelineReceiver<SourceInterface>, public PipelineEmitter<SourceInterface> {
public:

  /**
   * Constructor
   * @param thread_pool
   *    Alexandria thread pool
   */
  Prefetcher(const std::shared_ptr<Euclid::ThreadPool>& thread_pool, unsigned max_queue_size);

  /**
   * Destructor
   */
  virtual ~Prefetcher();

  /**
   * Trigger multi-threaded measurements on the source interface.
   * Once they are done, the message will be passed along.
   * @param message
   */
  void receiveSource(const std::shared_ptr<SourceInterface>& source) override;

  /**
   * Handle ProcessSourcesEvent. All sources received prior to this message need to
   * be processed before sources coming after are passed along.
   * @param message
   */
  void receiveProcessSignal(const ProcessSourcesEvent& event) override;

  /**
   * Tell the prefetcher to compute this property
   * @tparam Container
   *    Any iterable container with a set/list of properties
   * @param properties
   *    PropertyId instances
   */
  template<typename Container>
  void requestProperties(Container&& properties) {
    for (auto& p : properties) {
      requestProperty(p);
    }
  }

  /**
   * Wait for the multi-threaded computation to finish.
   * This must be done as the segmentation may be completely finished, and the measurement
   * queue empty, but some sources may still be here due to some compute-heavy property
   */
  void wait();

  /**
   * Wait until the queue is empty but don't stop the thread
   */

  void synchronize();


private:
  struct EventType {
    enum Type {
      SOURCE, PROCESS_SOURCE
    } m_event_type;
    intptr_t m_source_addr;

    explicit EventType(Type type, intptr_t source_addr = -1)
      : m_event_type(type), m_source_addr(source_addr) {}
  };

  /// Pointer to the pool of worker threads
  std::shared_ptr<Euclid::ThreadPool> m_thread_pool;
  /// Properties to prefetch
  std::set<PropertyId> m_prefetch_set;
  /// Orchestration thread
  std::unique_ptr<std::thread> m_output_thread;
  /// Notifies there is a new source done processing
  std::condition_variable m_new_output;
  /// Finished sources
  std::map<intptr_t, std::shared_ptr<SourceInterface>> m_finished_sources;
  /// Queue of received ProcessSourceEvent, order preserved
  std::deque<ProcessSourcesEvent> m_event_queue;
  /// Queue of type of received events. Used to pass downstream events respecting the received order
  std::deque<EventType> m_received;

  std::mutex m_queue_mutex;

  /// Termination condition for the output loop
  std::atomic_bool m_stop;

  /// Keep the queue under control
  Euclid::Semaphore m_semaphore;

  void requestProperty(const PropertyId& property_id);
  void outputLoop();
};

} // end of namespace SourceXtractor

#endif // _SEIMPLEMENTATION_MEASUREMENT_PREFETCHER_H_
