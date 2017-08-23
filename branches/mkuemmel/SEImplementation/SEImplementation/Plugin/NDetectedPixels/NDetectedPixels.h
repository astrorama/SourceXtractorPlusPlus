/**
 * @file SEImplementation/NDetectedPixels.h
 * @date 03/14/17
 * @author mkuemmel
 */
#ifndef _SEIMPLEMENTATION_PLUGIN_NDETECTEDPIXELS_H_
#define _SEIMPLEMENTATION_PLUGIN_NDETECTEDPIXELS_H_

#include "SEFramework/Property/Property.h"
namespace SExtractor {
class NDetectedPixels : public Property {
public:
  virtual ~NDetectedPixels() = default;
  NDetectedPixels(unsigned long n_detected_pixels) : m_n_detected_pixels(n_detected_pixels) {}
  long int getNDetectedPixels () const {
    return m_n_detected_pixels;
  }
private:
  long int m_n_detected_pixels;
}; // end of NDetectedPixels class
} // namespace SExtractor

#endif /* _SEIMPLEMENTATION_PLUGIN_NDETECTEDPIXELS_H_*/
