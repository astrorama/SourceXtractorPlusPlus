/*
 * Created on Jan 05, 2015
 * @author: mkuemmel@usm.lmu.de
 *
 * Date:     $Date$
 * Revision: $Revision$
 * Author:   $Author$
 */
#ifndef TYPEDSPLINEMODELWRAPPER_H
#define	TYPEDSPLINEMODELWRAPPER_H

#include <boost/filesystem.hpp>
//#include "SEFramework/Image/Image.h"
//#include "SEFramework/Image/ImageChunk.h"
#include "SEFramework/Image/ImageBase.h"
#include "SEImplementation/Background/SplineModel.h"

namespace SExtractor {

template <typename T>
class TypedSplineModelWrapper final : public ImageBase<T> {

public:

  //TypedSplineModelWrapper(const size_t* naxes, const size_t* gridCellSize, const size_t* nGrid, PIXTYPE* gridData){
  //  m_spline_model = new SplineModel(naxes, gridCellSize, nGrid, gridData);
  //};

  virtual ~TypedSplineModelWrapper(){
    if (m_spline_model){
      delete m_spline_model;
    }
  };

  static std::shared_ptr<TypedSplineModelWrapper<T>> create(const size_t* naxes, const size_t* gridCellSize, const size_t* nGrid, PIXTYPE* gridData) {
    return std::shared_ptr<TypedSplineModelWrapper<T>>(new TypedSplineModelWrapper<T>(naxes, gridCellSize, nGrid, gridData));
  }

  /// Returns the value of the pixel with the coordinates (x,y)
  T getValue(int x, int y) const override {
    return (T)m_spline_model->getValue((size_t)x, (size_t)y);
  };

  /// Returns the width of the image in pixels
  int getWidth() const override {
    return (int)(m_spline_model->getNaxes())[0];
  };

  /// Returns the height of the image in pixels
  int getHeight() const override {
    return (int)(m_spline_model->getNaxes())[1];
  };


private:
  TypedSplineModelWrapper(const size_t* naxes, const size_t* gridCellSize, const size_t* nGrid, PIXTYPE* gridData){
    m_spline_model = new SplineModel(naxes, gridCellSize, nGrid, gridData);
  };
  SplineModel* m_spline_model=nullptr;
};

} // end of namespace SExtractor

#endif	// TYPEDSPLINEMODELWRAPPER_H

