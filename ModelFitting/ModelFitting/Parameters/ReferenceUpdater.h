/** 
 * @file ReferenceUpdater.h
 * @date August 24, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef MODELFITTING_REFERENCEUPDATER_H
#define	MODELFITTING_REFERENCEUPDATER_H

#include "ModelFitting/Parameters/BasicParameter.h"

namespace ModelFitting {

class ReferenceUpdater {
  
public:
  
  using PreAction = std::function<void(double)>;
  using PostAction = std::function<void(double)>;
  
  ReferenceUpdater(BasicParameter& parameter, double& target,
                   PreAction pre_action={}, PostAction post_action={})
            : m_parameter(parameter), m_target(target), 
              m_pre_action{pre_action}, m_post_action{post_action} {
    if (m_parameter_alive) {
      m_observer_id = m_parameter.addObserver(Observer(*this));
    }
  }
            
  ReferenceUpdater(const ReferenceUpdater&) = delete;
  ReferenceUpdater& operator=(const ReferenceUpdater&) = delete;
  ReferenceUpdater& operator=(ReferenceUpdater&&) = delete;
  ReferenceUpdater(ReferenceUpdater&& other) = delete;
              
  ~ReferenceUpdater() {
    if (m_parameter_alive) {
      m_parameter.removeObserver(m_observer_id);
    }
  }
  
  bool isParameterAlive() const {
    return m_parameter_alive;
  }
  
  BasicParameter& getParameter() const {
    return m_parameter;
  }
  
private:
  
  BasicParameter& m_parameter;
  double& m_target;
  PreAction m_pre_action;
  PostAction m_post_action;
  bool m_parameter_alive = true;
  std::size_t m_observer_id {0};
  
  class Observer {
  public:
    Observer(ReferenceUpdater& owner) : m_owner(owner), alive_updater{new AliveUpdater{owner.m_parameter_alive}} { }
    void operator()(double v) {
      if (m_owner.m_pre_action) {
        m_owner.m_pre_action(v);
      }
      m_owner.m_target = v;
      if (m_owner.m_post_action) {
        m_owner.m_post_action(v);
      }
    }
  private:
    ReferenceUpdater& m_owner;
    class AliveUpdater {
    public:
      AliveUpdater(bool& flag) : m_flag(flag) { }
      ~AliveUpdater() {
        m_flag = false;
      }
    private:
      bool& m_flag;
    }; // end of class AliveUpdater
    
    std::shared_ptr<AliveUpdater> alive_updater;
  };
  
};

} // end of namespace ModelFitting

#endif	/* MODELFITTING_REFERENCEUPDATER_H */

