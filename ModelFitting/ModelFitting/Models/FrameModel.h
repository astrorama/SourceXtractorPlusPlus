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
#include "ModelFitting/Image/ImageTraits.h"
#include "ModelFitting/Image/PsfTraits.h"

namespace ModelFitting {

/**
 * Adapter class for "traditional" PSF types: those without a context
 * that can be computed only once (see prepare method on PSF types)
 * @tparam PsfType
 *  The wrapped PSF type
 */
template <typename PsfType>
class FrameModelPsfContainer: public PsfType {
public:

  /**
   * Constructor
   * @param n_extended_models
   *    Ignored for this implementation
   */
  FrameModelPsfContainer(std::size_t n_extended_models);

  /**
   * Constructor
   * @param psf
   *    Wrapped PSF
   * @param n_extended_models
   *    Ignored for this implementation
   */
  FrameModelPsfContainer(PsfType psf, std::size_t n_extended_models);

  /**
   * Wrap the call to the underlying PSF convolve method
   * @tparam ImageType
   *    The type of the image to be convolved. It has to be readable/writable
   * @param image
   *    The image to convolve
   */
  template <typename ImageType>
  void convolve(size_t, ImageType& image) {
    PsfType::convolve(image);
  }
};

/**
 * Adapter class for PSF types that have the concept of a context:
 * values that can be computed only once (see prepare method on PSF types)
 * @tparam PsfType
 *  The wrapped PSF type
 */
template <typename PsfType>
class FrameModelPsfContextContainer: public PsfType {
public:

  /**
   * Constructor
   * @param n_extended_models
   *    Only extended models are convolved. Allocate4 enough space for this number of contexts.
   */
  FrameModelPsfContextContainer(std::size_t n_extended_models);

  /**
   * Constructor
   * @param psf
   *    Wrapped PSF
   * @param n_extended_models
   *    Only extended models are convolved. Allocate4 enough space for this number of contexts.
   */
  FrameModelPsfContextContainer(PsfType psf, std::size_t n_extended_models);

  /**
   * Wrap the call to the underlying PSF prepare/convolve methods
   * @tparam ImageType
   *    The type of the image to be convolved. It has to be readable/writable
   * @param i
   *    The index of the model, which is used to retrieve the associated context
   * @param image
   *    The image to convolve
   */
  template <typename ImageType>
  void convolve(size_t i, ImageType& image) {
    auto& context = m_psf_contexts[i];
    if (!context) {
      context = PsfType::prepare(image);
    }
    PsfType::convolve(image, context);
  }

private:
  std::vector<typename PsfTraits<PsfType>::context_t> m_psf_contexts;
};


template <typename PsfType, typename ImageType>
class FrameModel {
private:

  // PsfTraits must have a has_context boolean with the value of true
  // if PsfType has a context type and a prepare method.
  // If it is the case, the PSF will be wrapped by FrameModelPsfContextContainer:
  // each model will have its own context.
  // Otherwise, the PSF will be just wrapped by FrameModelPsfContainer, which
  // forwards directly the calls.
  using psf_container_t = typename std::conditional<
    PsfTraits<PsfType>::has_context,
    FrameModelPsfContextContainer<PsfType>,
    FrameModelPsfContainer<PsfType>
  >::type;

public:

  using const_iterator = typename ImageTraits<ImageType>::iterator;
  
  FrameModel(double pixel_scale, std::size_t width, std::size_t height,
             std::vector<ConstantModel> constant_model_list,
             std::vector<PointModel> point_model_list,
             std::vector<std::shared_ptr<ExtendedModel<ImageType>>> extended_model_list,
             PsfType psf);
  
  FrameModel(double pixel_scale, std::size_t width, std::size_t height,
             std::vector<ConstantModel> constant_model_list,
             std::vector<PointModel> point_model_list,
             std::vector<std::shared_ptr<ExtendedModel<ImageType>>> extended_model_list);


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
  std::vector<std::shared_ptr<ExtendedModel<ImageType>>> m_extended_model_list;
  psf_container_t m_psf;
  std::unique_ptr<ImageType> m_model_image {};
  
}; // end of class FrameModel

} // end of namespace ModelFitting

#include "_impl/FrameModel.icpp"

#endif	/* MODELFITTING_FRAMEMODEL_H */

