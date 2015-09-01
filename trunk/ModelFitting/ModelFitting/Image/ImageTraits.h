/** 
 * @file ImageTraits.h
 * @date August 27, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef MODELFITTING_IMAGETRAITS_H
#define	MODELFITTING_IMAGETRAITS_H

namespace ModelFitting {

template <typename ImageType>
struct ImageTraits {
  
  class iterator;
  
  static ImageType factory(std::size_t width, std::size_t height);
  
  static std::size_t width(ImageType& image);
  
  static std::size_t height(ImageType& image);
  
  static double& at(ImageType& image, std::size_t x, std::size_t y);
  
  static iterator begin(ImageType& image);
  
  static iterator end(ImageType& image);
  
  static void addImageToImage(ImageType& image1, double scale1,
                              const ImageType& image2, double scale2,
                              double x, double y);
  
}; // end of class ImageTraits

} // end of namespace ModelFitting

#endif	/* MODELFITTING_IMAGETRAITS_H */

