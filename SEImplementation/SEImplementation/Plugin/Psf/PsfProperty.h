/*
 * PsfProperty.h
 *
 *  Created on: Jun 25, 2018
 *      Author: Alejandro Álvarez Ayllón
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_PSF_PSF_H_
#define _SEIMPLEMENTATION_PLUGIN_PSF_PSF_H_

#include <SEFramework/Property/Property.h>
#include <SEFramework/Image/VectorImage.h>

namespace SExtractor {

class PsfProperty : public Property {
public:
  virtual ~PsfProperty() = default;

  PsfProperty(double pixel_sampling, std::shared_ptr<VectorImage <SeFloat>> psf) :
    m_pixel_sampling(pixel_sampling), m_psf(psf) {};

  PsfProperty();

  double getPixelSampling() const {
    return m_pixel_sampling;
  }

  std::shared_ptr<VectorImage<SeFloat>> getPsf() const {
    return m_psf;
  }

private:
  double m_pixel_sampling;
  std::shared_ptr<VectorImage<SeFloat>> m_psf;
};

} // end SExtractor

#endif //_SEIMPLEMENTATION_PLUGIN_PSF_PSF_H_
