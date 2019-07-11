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
#include "SEFramework/Image/ImageBase.h"
#include "SEFramework/Image/ImageSource.h"
#include "SEImplementation/Background/SplineModel.h"

namespace SExtractor {

template <typename T>
class TypedSplineModelWrapper final : public ImageSource<T> {

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

  /// Human readable representation
  std::string getRepr() const override {
    return "TypedSplineModel";
  }

  /// Returns the value of the pixel with the coordinates (x,y)
  T getValue(int x, int y) const {
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

  /// Returns the median of the spline
  T getMedian() const {
    return (T)m_spline_model->getMedian();
  };

  std::shared_ptr<ImageTile<T>> getImageTile(int x, int y, int width, int height) const override {
    auto tile = std::make_shared<ImageTile<T>>(x, y, width, height);
    // Splines are calculated and cached per row. We fill
    // the tile with the Y axis on the outer loop, so we can
    // benefit from that caching
    // @see SplineModel::getValue
    for (auto j = y; j < y + height; ++j) {
      for (auto i = x; i < x + width; ++i) {
        tile->setValue(i, j, getValue(i, j));
      }
    }
    return tile;
  }

  void saveTile(ImageTile<T>& /*tile*/) override {
    assert(false);
  }

private:
  TypedSplineModelWrapper(const size_t* naxes, const size_t* gridCellSize, const size_t* nGrid, PIXTYPE* gridData){
    m_spline_model = new SplineModel(naxes, gridCellSize, nGrid, gridData);
  };
  SplineModel* m_spline_model=nullptr;
};

} // end of namespace SExtractor

#endif	// TYPEDSPLINEMODELWRAPPER_H

