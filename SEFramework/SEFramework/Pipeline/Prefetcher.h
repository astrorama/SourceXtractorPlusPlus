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
#include "SEUtils/Observable.h"

namespace SourceXtractor {

/**
 * The pre-fetcher allows later stages, as the grouping or the cleaning,
 * to ask in advance for some compute intensive properties, so they can be done
 * multi-threaded before it reaches them
 */
class Prefetcher : public Observer<std::shared_ptr<SourceInterface>>,
                   public Observable<std::shared_ptr<SourceInterface>> {
public:

  Prefetcher(const std::shared_ptr<Euclid::ThreadPool>& thread_pool);

  virtual ~Prefetcher();

  void handleMessage(const std::shared_ptr<SourceInterface>& message) override;

  template<typename PropertyType>
  void requestProperty(unsigned int index = 0) {
    static_assert(std::is_base_of<Property, PropertyType>::value, "PropertyType must inherit from SourceXtractor::Property");
    requestProperty(PropertyId::create<PropertyType>(index));
  }

  template<typename Container>
  void requestProperties(Container&& properties) {
    for (auto& p : properties) {
      requestProperty(p);
    }
  }

private:
  std::shared_ptr<Euclid::ThreadPool> m_thread_pool;
  std::set<PropertyId> m_prefetch_set;
  std::unique_ptr<std::thread> m_output_thread;
  std::condition_variable m_new_output;
  std::deque<std::shared_ptr<SourceInterface>> m_output_queue;
  std::mutex m_output_queue_mutex;
  std::atomic_bool m_stop;

  void requestProperty(const PropertyId& property_id);
  void outputLoop();
};

} // end of namespace SourceXtractor

#endif // _SEIMPLEMENTATION_MEASUREMENT_PREFETCHER_H_
