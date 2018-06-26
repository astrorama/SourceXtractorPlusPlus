/*
 * VariablePsf.h
 *
 *  Created on: Jun 25, 2018
 *      Author: Alejandro Álvarez Ayllón
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_PSF_PSF_H_
#define _SEIMPLEMENTATION_PLUGIN_PSF_PSF_H_

#include <SEFramework/Property/Property.h>
#include <SEFramework/Image/VectorImage.h>
#include <SEImplementation/Image/ImagePsf.h>

namespace SExtractor {

class PsfProperty: public Property {
public:
  virtual ~PsfProperty() = default;
  PsfProperty(ImagePsf &&psf): m_psf(std::move(psf)) {};

  PsfProperty();

  const ImagePsf& getPsf() const {
    return m_psf;
  }

private:
  ImagePsf m_psf;
};

} // end SExtractor

#endif //_SEIMPLEMENTATION_PLUGIN_PSF_PSF_H_
