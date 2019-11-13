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
  
  static double at(const ImageType& image, std::size_t x, std::size_t y);

  static iterator begin(ImageType& image);
  
  static iterator end(ImageType& image);
  
  static void addImageToImage(ImageType& image1,
                              const ImageType& image2, double scale,
                              double x, double y);
  
}; // end of class ImageTraits

} // end of namespace ModelFitting

#endif	/* MODELFITTING_IMAGETRAITS_H */

