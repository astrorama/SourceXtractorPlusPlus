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
#include "SEFramework/Source/SourceInterface.h"
#include "SEFramework/Pipeline/SourceGrouping.h"
#include "SEUtils/Observable.h"

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
class Prefetcher : public Observer<std::shared_ptr<SourceInterface>>,
                   public Observable<std::shared_ptr<SourceInterface>>,
                   public Observer<ProcessSourcesEvent>,
                   public Observable<ProcessSourcesEvent> {
public:

  /**
   * Constructor
   * @param thread_pool
   *    Alexandria thread pool
   */
  Prefetcher(const std::shared_ptr<Euclid::ThreadPool>& thread_pool);

  /**
   * Destructor
   */
  virtual ~Prefetcher();

  /**
   * Trigger multi-threaded measurements on the source interface.
   * Once they are done, the message will be passed along.
   * @param message
   */
  void handleMessage(const std::shared_ptr<SourceInterface>& message) override;

  /**
   * Handle ProcessSourcesEvent. All sources received prior to this message need to
   * be processed before sources coming after are passed along.
   * @param message
   */
  void handleMessage(const ProcessSourcesEvent& message) override;

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

private:
  std::shared_ptr<Euclid::ThreadPool> m_thread_pool;
  std::set<PropertyId> m_prefetch_set;
  std::unique_ptr<std::thread> m_output_thread;
  std::condition_variable m_new_output;
  std::list<std::shared_ptr<SourceInterface>> m_output_queue;
  std::atomic_int64_t m_last_received;
  std::set<int64_t> m_ongoing;
  std::deque<std::pair<int64_t, ProcessSourcesEvent>> m_event_queue;
  std::mutex m_output_queue_mutex, m_ongoing_mutex;
  std::atomic_bool m_stop;

  void requestProperty(const PropertyId& property_id);
  void outputLoop();
};

} // end of namespace SourceXtractor

#endif // _SEIMPLEMENTATION_MEASUREMENT_PREFETCHER_H_
