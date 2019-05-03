/*
 * @file SEFramework/Convolution/DirectConvolution.h
 * @date 17/09/18
 * @author aalvarez
 */

#ifndef _SEFRAMEWORK_CONVOLUTION_DIRECTCONVOLUTION_H
#define _SEFRAMEWORK_CONVOLUTION_DIRECTCONVOLUTION_H

#include "SEFramework/Image/PaddedImage.h"
#include "SEFramework/Image/VectorImage.h"

namespace SExtractor {

template <typename T = SeFloat, class TPadding = PaddedImage<T, Reflect101Coordinates>>
class DirectConvolution {
public:
  DirectConvolution(std::shared_ptr<const Image<T>> img)
    : m_kernel{img} {
  }

  virtual ~DirectConvolution() = default;

  template <typename ...Args>
  void convolve(std::shared_ptr<WriteableImage<T>> image, Args... padding_args) const {
    auto padded_width = image->getWidth() + m_kernel->getWidth() - 1;
    auto padded_height = image->getHeight() + m_kernel->getHeight() - 1;
    auto lpad = m_kernel->getWidth() / 2;
    auto tpad = m_kernel->getHeight() / 2;

    auto padded = VectorImage<T>::create(
      TPadding::create(image, padded_width, padded_height, std::forward<Args>(padding_args)...)
    );

    for (int iy = tpad; iy < padded->getHeight() - tpad; ++iy) {
      for (int ix = lpad; ix < padded->getWidth() - lpad; ++ix) {
        T acc = 0;
        for (int ky = 0; ky < m_kernel->getHeight(); ++ky) {
          for (int kx = 0; kx < m_kernel->getWidth(); ++kx) {
            acc += m_kernel->getValue(kx, ky) * padded->getValue(ix - lpad + kx, iy - tpad + ky);
          }
        }
        image->setValue(ix - lpad, iy - tpad, acc);
      }
    }
  }

  std::size_t getWidth() const {
    return m_kernel->getWidth();
  }

  std::size_t getHeight() const {
    return m_kernel->getHeight();
  }

  std::shared_ptr<const Image<T>> getKernel() const {
    return m_kernel;
  }

private:
  std::shared_ptr<const Image<T>> m_kernel;
};

} // end SExtractor

#endif // _SEFRAMEWORK_CONVOLUTION_DIRECTCONVOLUTION_H
