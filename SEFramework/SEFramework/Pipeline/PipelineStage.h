/** Copyright © 2022 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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

#ifndef _SEFRAMEWORK_PIPELINE_PIPELINESTAGE_H
#define _SEFRAMEWORK_PIPELINE_PIPELINESTAGE_H

#include "SEFramework/Source/SourceInterface.h"
#include "SEUtils/Observable.h"

namespace SourceXtractor {

class SelectionCriteria;

/**
 * @class ProcessSourcesEvent
 * @brief Event received by SourceGrouping to request the processing of some of the Sources stored.
 *
 */
struct ProcessSourcesEvent {

  const std::shared_ptr<SelectionCriteria> m_selection_criteria;  // Used to identify the Sources to process

  explicit ProcessSourcesEvent(std::shared_ptr<SelectionCriteria> selection_criteria)
      : m_selection_criteria(std::move(selection_criteria)) {}
};

/**
 * Receiver component of a pipeline.
 * @tparam T
 */
template <typename T>
class PipelineReceiver {
public:
  virtual ~PipelineReceiver() = default;

  /**
   * Receive a source from the previous stage. The receiver owns the object.
   * @param source
   */
  virtual void receiveSource(std::unique_ptr<T> source)        = 0;

  /**
   * Receive a signal to process sources when grouping
   * @param event
   */
  virtual void receiveProcessSignal(const ProcessSourcesEvent& event) = 0;
};

/**
 * Emitter component of a pipeline
 * @tparam T
 */
template <typename T>
class PipelineEmitter : public Observable<T> {
public:
  ~PipelineEmitter() override = default;

  /**
   * Set the next stage of the pipeline. There can be *only one*, so there is a single
   * pipeline stage owning the object.
   * @param next
   */
  void setNextStage(std::shared_ptr<PipelineReceiver<T>> next) {
    if (m_next_stage) {
      throw Elements::Exception() << "Next stage already set";
    }
    m_next_stage = std::move(next);
  }

protected:
  void sendSource(std::unique_ptr<T> source) const {
    Observable<T>::notifyObservers(*source);
    if (m_next_stage) {
      m_next_stage->receiveSource(std::move(source));
    }
  }

  void sendProcessSignal(const ProcessSourcesEvent& event) const {
    if (m_next_stage) {
      m_next_stage->receiveProcessSignal(event);
    }
  }

private:
  std::shared_ptr<PipelineReceiver<T>> m_next_stage;
};

}  // namespace SourceXtractor

#endif  // _SEFRAMEWORK_PIPELINE_PIPELINESTAGE_H
