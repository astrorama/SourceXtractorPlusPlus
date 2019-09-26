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