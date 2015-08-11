/**
 * @file ParameterWatcher.h
 *
 * Created on: July 28, 2015
 *     Author: Pierre Dubath
 */

#ifndef PARAMETER_WATCHER_H_
#define PARAMETER_WATCHER_H_

namespace ModelFitting {

/**
 * @class ParameterWatcher
 * @brief
 *    The parameter update notification interface
 *
 * @details
 *    This interface can be implemented to receive notifications, with the new
 *    value of any changes of a parameter value. ParameterWatcher can be associated
 *    with a particular BasicParameter using the addParameterWather() method of
 *    the BasicParameter class
 *
 */
class ParameterWatcher {

public:
  /**
    * @brief
    *    The pure virtual updatedValue(double new_value) method
    * @details
    *    The implementation must define the operations that the
    *    ParameterWather wants to carry out when the parameter
    *    value is updated. and provided through this method.
    *
    * @param new_value
    *    The updated value of the parameter
    *
    */
   virtual void updatedValue(double new_value) = 0;

  /**
   * @brief Destructor
   */
  virtual ~ParameterWatcher() {
  }

};

} // namespace ModelFitting

#endif /* PARAMETER_WATCHER_H_ */
