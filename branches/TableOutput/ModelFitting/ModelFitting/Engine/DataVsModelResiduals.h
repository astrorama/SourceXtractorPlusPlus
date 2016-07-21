/** 
 * @file DataVsModelResiduals.h
 * @date August 19, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef MODELFITTING_DATAVSMODELRESIDUALS_H
#define	MODELFITTING_DATAVSMODELRESIDUALS_H

#include <memory>
#include "ElementsKernel/Exception.h"
#include "ModelFitting/Engine/ResidualBlockProvider.h"
#include "ModelFitting/Engine/DataVsModelInputTraits.h"

namespace ModelFitting {

/**
 * @class DataVsModelResiduals
 * 
 * @brief
 * ResidualBlockProvider for weighted comparison between data and a model
 * 
 * @details
 * The residuals are computed using the following equation:
 * \f[ r_i = g_{(d_i, m_i, w_i)} \f]
 * where:
 * - \f$g\f$: is a user defined cost function (for example \f$\chi^2\f$
 * - \f$d_i\f$: is the data point with index \f$i\f$
 * - \f$m_i\f$: is the equivalent model value
 * - \f$w_i\f$: is the weight of the data point
 * 
 * This class is designed to be as flexible as possible, so it can be used for
 * comparing a wide range or data types (arrays, images, etc), with different
 * cost functions. This flexibility is achieved in two ways.
 * 
 * First, the cost method (the way the residuals are computed) can be defined by
 * the user by using the Comparator template parameter. The Comparator can be
 * any type which behaves like a function (function pointer, functor, lambda,
 * etc) as long as it has the signature void(double d, double m, double w).
 * 
 * Second, the types of the classes which provide the data, model and weight
 * values are all templated, so any type can be used. To be fully flexible, the
 * DataVsModelResiduals class does not access directly these type methods to
 * not imply any restrictions on the possible container types. Instead, it uses
 * the DataVsModelInputTraits class, which is responsible for redirecting the
 * calls to the underlying type. Note that the default implementation of this
 * class covers all the STL containers, so they can be used out-of-the-box. To
 * use incompatible underlying types, one just needs to create a specialization
 * of the DataVsModelInputTraits (see the DataVsModelInputTraits documentation
 * for more details).
 * 
 * @tparam DataType
 *    The type used for accessing the data point values
 * @tparam ModelType
 *    The type used for accessing the model values
 * @tparam WeightType
 *    The type used for accessing the weight values
 * @tparam Comparator
 *    The function type to use for computing the residuals
 */
template <typename DataType, typename ModelType, typename WeightType, typename Comparator>
class DataVsModelResiduals : public ResidualBlockProvider {
  
  // The following are just aliases to reduce the typing and make the code
  // more readable
  using DataTraits = DataVsModelInputTraits<DataType>;
  using ModelTraits = DataVsModelInputTraits<ModelType>;
  using WeightTraits = DataVsModelInputTraits<WeightType>;
  
public:
  
  /**
   * @brief Constructs a new instance of DataVsModelResiduals
   * 
   * @details
   * The DataVsModelResiduals keeps internally copies of all the parameters of
   * the constructor. Modifications done after the DataVsModelResiduals is
   * created will not be visible by the DataVsModelResiduals instance.
   * 
   * @param data
   *    The data to compute the residuals for
   * @param model
   *    The model values for the same points as the data
   * @param weight
   *    The  weights of the data points
   * @param comparator
   *    The function to use for computing the residuals
   * @throws ElementsException
   *    If the data, model or weight have different sizes
   */
  DataVsModelResiduals(DataType data, ModelType model, WeightType weight, Comparator comparator);
  
  /// Destructor
  virtual ~DataVsModelResiduals();
  
  /// Returns the number of residuals produced by this residual provider (same
  /// as the number of data points)
  std::size_t numberOfResiduals() const override;
  
  /// Updates the values where the iterator points with the residuals
  void populateResidualBlock(IterType output_iter) override;
  
private:
  
  DataType m_data;
  ModelType m_model;
  WeightType m_weight;
  Comparator m_comparator;
  std::size_t m_residual_no;
  
}; // end of class DataVsModelResiduals


/// Factory method for creating DataVsModelResiduals. This method exists to avoid
/// typing the template types required when calling the DataVsModelResiduals
/// constructor (because C++ cannot infer them from the parameters). Note that
/// this method uses perfect forwarding, so its calls, with respect with rvalue -
/// lvalue logic, have the same behavior with direct calls of the constructor.
///
/// Don't get scared by the template parameters. They are all inferred from the
/// arguments of the method and are completely transparent when calling the
/// factory.
template <typename DataType, typename ModelType, typename WeightType, typename Comparator>
std::unique_ptr<DataVsModelResiduals<typename std::remove_reference<DataType>::type,
                                     typename std::remove_reference<ModelType>::type,
                                     typename std::remove_reference<WeightType>::type,
                                     typename std::remove_reference<Comparator>::type>
               > createDataVsModelResiduals(DataType&& data, ModelType&& model,
                                            WeightType&& weight, Comparator&& comparator);

} // end of namespace ModelFitting

#include "_impl/DataVsModelResiduals.icpp"

#endif	/* MODELFITTING_DATAVSMODELRESIDUALS_H */

