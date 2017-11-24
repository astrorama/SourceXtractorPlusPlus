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

//#include <cstddef>
#include <boost/filesystem.hpp>
#include "SEFramework/Image/Image.h"
#include "SEFramework/Image/ImageChunk.h"
#include "SEFramework/Image/ImageBase.h"
//#include "SEImplementation/Background/BackgroundDefine.h"
#include "SEImplementation/Background/SplineModel.h"

namespace SExtractor {

template <typename T>
class TypedSplineModelWrapper final : public ImageBase<T> {

public:

  TypedSplineModelWrapper(const size_t* naxes, const size_t* gridCellSize, const size_t* nGrid, PIXTYPE* gridData){
    m_spline_model = new SplineModel(naxes, gridCellSize, nGrid, gridData);
  };

  //TypedSplineModelWrapper(const boost::filesystem::path modelFile);
  virtual ~TypedSplineModelWrapper(){
    if (m_spline_model){
      delete m_spline_model;
    }
  };

  //static std::shared_ptr<TypedSplineModelWrapper<T>> create(Args&&... args) {
  //  return std::shared_ptr<TypedSplineModelWrapper<T>>(new TypedSplineModelWrapper<T>(std::forward<Args>(args)...));
  //}
  static std::shared_ptr<TypedSplineModelWrapper<T>> create(const size_t* naxes, const size_t* gridCellSize, const size_t* nGrid, PIXTYPE* gridData) {
    return std::shared_ptr<TypedSplineModelWrapper<T>>(new TypedSplineModelWrapper<T>(naxes, gridCellSize, nGrid, gridData));
  }

  void splineLine(PIXTYPE *line, const size_t y, const size_t xStart, const size_t width) const {
    return m_spline_model->splineLine(line,  y, xStart, width);
  };

  /// Returns the value of the pixel with the coordinates (x,y)
  T getValue(int x, int y) const override {
    // TODO: push the buffering into SplineModel

    T rValue;
    PIXTYPE* back_line = new PIXTYPE[getWidth()];

    // compute values for the current line
    splineLine(back_line, (size_t)y, 0,(size_t)getWidth());

    // extract the return value
    rValue = (T)back_line[x];
    ///return (T)(0.0);

    // mop up and return value
    delete back_line;
    return rValue;
  };

  /// Returns the width of the image in pixels
  int getWidth() const override {
    return (int)(m_spline_model->getNaxes())[0];
  };

  /// Returns the height of the image in pixels
  int getHeight() const override {
    return (int)(m_spline_model->getNaxes())[1];
  };

  //virtual std::shared_ptr<ImageChunk<T>> getChunk(int x, int y, int width, int height){
  //  m_chunk_data.resize(height*width, (T)0.0);
  //  //return ImageChunk<T>::create(&m_data[x + y * m_width], width, height, m_width, this->shared_from_this());
  //  return ImageChunk<T>::create(&m_chunk_data[0], width, height, 1, this->shared_from_this());
  //};

private:
  SplineModel* m_spline_model=nullptr;
  std::vector<T> m_chunk_data;
};

} // end of namespace SExtractor

#endif	// TYPEDSPLINEMODELWRAPPER_H

