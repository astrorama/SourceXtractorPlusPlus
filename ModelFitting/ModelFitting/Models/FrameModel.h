/** 
 * @file FrameModel.h
 * @date August 27, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef MODELFITTING_FRAMEMODEL_H
#define	MODELFITTING_FRAMEMODEL_H

#include <vector>
#include <cmath>
#include "ModelFitting/Models/ConstantModel.h"
#include "ModelFitting/Models/PointModel.h"
#include "ModelFitting/Models/ExtendedModel.h"
#include "ModelFitting/Models/TransformedModel.h"
#include "ModelFitting/Image/ImageTraits.h"

namespace ModelFitting {

template <typename PsfType, typename ImageType>
class FrameModel {
  
public:
  
  using const_iterator = typename ImageTraits<ImageType>::iterator;
  
  FrameModel(double pixel_scale, std::size_t width, std::size_t height,
             std::vector<ConstantModel> constant_model_list,
             std::vector<PointModel> point_model_list,
             std::vector<TransformedModel> extended_model_list,
             PsfType psf);
  
  FrameModel(FrameModel&&) = default;
  
  virtual ~FrameModel();
  
  void recomputeImage();
  
  const ImageType& getImage();

  void rasterToImage(ImageType&);
  
  const_iterator begin();
  
  const_iterator end();
  
  std::size_t size() const;
  
private:
  
  double m_pixel_scale;
  std::size_t m_width;
  std::size_t m_height;
  std::vector<ConstantModel> m_constant_model_list;
  std::vector<PointModel> m_point_model_list;
  std::vector<TransformedModel> m_extended_model_list;
  PsfType m_psf;
  std::unique_ptr<ImageType> m_model_image {};
  
}; // end of class FrameModel

} // end of namespace ModelFitting

#include "_impl/FrameModel.icpp"

#endif	/* MODELFITTING_FRAMEMODEL_H */

