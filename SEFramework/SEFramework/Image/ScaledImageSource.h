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

#ifndef _SEFRAMEWORK_IMAGE_SCALEDIMAGESOURCE_H
#define _SEFRAMEWORK_IMAGE_SCALEDIMAGESOURCE_H

#include "SEFramework/Image/ImageSource.h"
#include "MathUtils/interpolation/interpolation.h"

namespace SourceXtractor {

/**
 * Scales an image to a target width and height.
 * Note that the original image grid is centered on the target one, so values on the edges are actually
 * extrapolated.
 * @tparam T
 *  Pixel type
 */
template<typename T>
class ScaledImageSource : public ImageSource {
public:
  /// Interpolation type: bilinear or bicubic
  enum class InterpolationType {
    BILINEAR, BICUBIC
  };

  /**
   * Constructor
   * @param image
   *    Image to upscale
   * @param width
   *    New width
   * @param height
   *    New height
   * @param interp_type
   *    Interpolation type
   */
  ScaledImageSource(const std::shared_ptr<Image<T>>& image, int width, int height, InterpolationType interp_type = InterpolationType::BICUBIC)
    : m_image(image), m_width(width), m_height(height) {
    m_wscale = std::ceil(static_cast<float>(width) / image->getWidth());
    m_hscale = std::ceil(static_cast<float>(height) / image->getHeight());

    switch (interp_type) {
      case InterpolationType::BICUBIC:
        m_interpolation_type = Euclid::MathUtils::InterpolationType::CUBIC_SPLINE;
        break;
      case InterpolationType::BILINEAR:
        m_interpolation_type = Euclid::MathUtils::InterpolationType::LINEAR;
        break;
    }

    // Generate y coordinates on the original image
    std::vector<double> y_coords(image->getHeight());
    for (size_t i = 0; i < y_coords.size(); ++i) {
      y_coords[i] = std::floor((i + 0.5) * m_hscale);
    }

    // Generate x coordinates on the original image
    m_x_coords.resize(image->getWidth());
    for (size_t i = 0; i < m_x_coords.size(); ++i) {
      m_x_coords[i] = std::floor((i + 0.5) * m_wscale);
    }

    // Store interpolation along columns
    m_interpolated_cols.reserve(image->getWidth());
    for (int x = 0; x < image->getWidth(); ++x) {
      std::vector<double> values(image->getHeight());
      for (int y = 0; y < image->getHeight(); ++y) {
        values[y] = image->getValue(x, y);
      }
      m_interpolated_cols.emplace_back(
        Euclid::MathUtils::interpolate(y_coords, values, m_interpolation_type, true));
    }
  }

  /**
   * Destructor
   */
  virtual ~ScaledImageSource() = default;

  /**
   * @return Human readable representation of this object
   */
  std::string getRepr() const final {
    return std::string("ScaledImageSource");
  }

  /**
   * Get a tile from the scaled image
   * @param x
   *    Tile offset on the X coordinate
   * @param y
   *    Tile offset on the Y coordinate
   * @param width
   *    Tile width
   * @param height
   *    Tile height
   * @return
   *    An initialized upscaled tile
   */
  std::shared_ptr<ImageTile> getImageTile(int x, int y, int width, int height) const final {
    auto tile = ImageTile::create(ImageTile::getTypeValue(T()), x, y, width, height);

    for (int off_y = 0; off_y < height; ++off_y) {
      std::vector<double> v(m_x_coords);
      for (size_t ix = 0; ix < m_x_coords.size(); ++ix) {
        auto& fy = *m_interpolated_cols[ix];
        v[ix] = fy(y + off_y);
      }
      auto fx = Euclid::MathUtils::interpolate(m_x_coords, v, m_interpolation_type, true);
      for (int off_x = 0; off_x < width; ++off_x) {
        tile->setValue(x + off_x, y + off_y, T((*fx)(x + off_x)));
      }
    }
    return tile;
  }

  /**
   * A tile of this type can not be saved
   */
  void saveTile(ImageTile&) final {
    assert(false);
  }

  /**
   * @return Upscaled width
   */
  int getWidth() const final {
    return m_width;
  }

  /**
   * @return Upscaled height
   */
  int getHeight() const final {
    return m_height;
  }

  ImageTile::ImageType getType() const override {
    return ImageTile::getTypeValue(T());
  }

private:
  std::shared_ptr<Image<T>> m_image;
  int m_width, m_height;
  Euclid::MathUtils::InterpolationType m_interpolation_type;
  std::vector<std::unique_ptr<Euclid::MathUtils::Function>> m_interpolated_cols;
  std::vector<double> m_x_coords;
  double m_wscale, m_hscale;
};

} // end of namespace SourceXtractor

#endif // _SEFRAMEWORK_IMAGE_SCALEDIMAGESOURCE_H
