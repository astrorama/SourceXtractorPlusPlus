/** 
 * @file PointModel.cpp
 * @date September 1, 2015
 * @author Nikolaos Apostolakos
 */

#include "ModelFitting/Models/PointModel.h"

namespace ModelFitting {

PointModel::PointModel(BasicParameter& x, BasicParameter& y, BasicParameter& value)
        : PositionedModel(x, y), ConstantModel(value) { }

PointModel::PointModel(PointModel&& other)
        : PositionedModel(std::move(other)), ConstantModel(std::move(other)) { }

PointModel::~PointModel() = default;

} // end of namespace ModelFitting