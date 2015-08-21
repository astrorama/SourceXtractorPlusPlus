/** 
 * @file ResidualEstimator.cpp
 * @date August 17, 2015
 * @author Nikolaos Apostolakos
 */

#include "ModelFitting/Engine/ResidualEstimator.h"

namespace ModelFitting {

ResidualEstimator::~ResidualEstimator() = default;

void ResidualEstimator::registerBlockProvider(std::unique_ptr<ResidualBlockProvider> provider) {
  m_residual_no += provider->numberOfResiduals();
  m_block_provider_list.push_back(std::move(provider));
}

std::size_t ResidualEstimator::numberOfResiduals() const {
  return m_residual_no;
}

void ResidualEstimator::populateResiduals(double* output_iter) const {
  for (auto& block_prov_ptr : m_block_provider_list) {
    block_prov_ptr->populateResidualBlock(output_iter);
    output_iter += block_prov_ptr->numberOfResiduals();
  }
}

void ResidualEstimator::populateResiduals(std::vector<double>::iterator output_iter) const {
  for (auto& block_prov_ptr : m_block_provider_list) {
    block_prov_ptr->populateResidualBlock(&*output_iter);
    output_iter += block_prov_ptr->numberOfResiduals();
  }
}

} // end of namespace ModelFitting