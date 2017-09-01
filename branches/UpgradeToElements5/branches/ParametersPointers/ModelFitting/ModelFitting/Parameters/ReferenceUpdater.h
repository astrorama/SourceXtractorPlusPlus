/** 
 * @file ReferenceUpdater.h
 * @date August 24, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef MODELFITTING_REFERENCEUPDATER_H
#define	MODELFITTING_REFERENCEUPDATER_H

#include <memory>
#include <functional>
#include "ModelFitting/Parameters/BasicParameter.h"

namespace ModelFitting {

class ReferenceUpdater {
  
public:
  
  using PreAction = std::function<void(double)>;
  using PostAction = std::function<void(double)>;
  
  ReferenceUpdater(std::shared_ptr<BasicParameter> parameter, double& target,
                   PreAction pre_action={}, PostAction post_action={})
            : m_parameter{parameter}, m_target(target),
              m_pre_action{pre_action}, m_post_action{post_action} {
    addObserverToParameter();
  }
            
  ReferenceUpdater(const ReferenceUpdater&) = delete;
  ReferenceUpdater& operator=(const ReferenceUpdater&) = delete;
  ReferenceUpdater& operator=(ReferenceUpdater&&) = delete;
  ReferenceUpdater(ReferenceUpdater&& other) = delete;

  ~ReferenceUpdater() {
    if (auto param = m_parameter.lock()) {
      param->removeObserver(m_observer_id);
    }
  }
  
  std::weak_ptr<BasicParameter> getParameter() const {
    return m_parameter;
  }
  
private:
  
  std::weak_ptr<BasicParameter> m_parameter;
  double& m_target;
  std::size_t m_observer_id {0};
  PreAction m_pre_action;
  PostAction m_post_action;
  
  void addObserverToParameter() {
    if (auto parameter = m_parameter.lock()) {
      m_observer_id = parameter->addObserver(
        [this](double v) {
          if (m_pre_action) {
            m_pre_action(v);
          }
          m_target = v;
          if (m_post_action) {
            m_post_action(v);
          }
        }
      );
    }
  }
  
};

}

#endif	/* MODELFITTING_REFERENCEUPDATER_H */

