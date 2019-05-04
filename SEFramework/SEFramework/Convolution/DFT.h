/*
 * @file SEFramework/Convolution/DFT.h
 * @date 17/09/18
 * @author aalvarez
 */

#ifndef _SEFRAMEWORK_CONVOLUTION_DFT_H
#define _SEFRAMEWORK_CONVOLUTION_DFT_H

#include "SEFramework/Image/PaddedImage.h"
#include "SEFramework/Image/MirrorImage.h"
#include "SEFramework/Image/RecenterImage.h"
#include "SEFramework/FFT/FFT.h"

#include <fftw3.h>
#include <boost/thread/shared_mutex.hpp>


namespace SExtractor {


template<typename T = SeFloat, class TPadding = PaddedImage<T, Reflect101Coordinates>>
class DFTConvolution {
public:
  typedef T real_t;
  typedef typename FFT<T>::complex_t complex_t;

  DFTConvolution(std::shared_ptr<const Image<T>> img)
    : m_kernel{img} {
  }

  virtual ~DFTConvolution() = default;

  std::size_t getWidth() const {
    return m_kernel->getWidth();
  }

  std::size_t getHeight() const {
    return m_kernel->getHeight();
  }

  template <typename ...Args>
  void convolve(std::shared_ptr<WriteableImage<T>> image_ptr, Args... padding_args) const {
    // Dimension of the working padded images
    auto padded_width = image_ptr->getWidth() + m_kernel->getWidth() - 1;
    auto padded_height = image_ptr->getHeight() + m_kernel->getHeight() - 1;

    // For performance, use a size that is convenient for FFTW
    padded_width = fftRoundDimension(padded_width);
    padded_height = fftRoundDimension(padded_height);

    auto total_size = padded_height * padded_width;

    // Padded image
    auto padded = TPadding::create(image_ptr, padded_width, padded_height, std::forward<Args>(padding_args)...);

    // Buffers
    std::vector<real_t> real_buffer(total_size * 2);
    std::vector<complex_t> complex_buffer(total_size * 2);

    // Get the plans now, so these buffers can be used for the planning
    auto fwd_plan = FFT<T>::createForwardPlan(2, padded_width, padded_height, real_buffer, complex_buffer);
    auto idft_plan = FFT<T>::createInversePlan(1, padded_width, padded_height, complex_buffer, real_buffer);

    // Create a matrix with the padded image
    dumpImage(padded, real_buffer.begin());

    // Pad, center and mirror the kernel, centering at 0, 0 and  wrapping around the coordinates
    padMirroredKernel(padded_width, padded_height, real_buffer.begin() + total_size);

    FFT<T>::executeForward(fwd_plan, real_buffer, complex_buffer);

    // Multiply the two DFT
    for (int i = 0; i < total_size; ++i) {
      complex_buffer[i] *= complex_buffer[i + total_size];
    }

    // Inverse DFT
    FFT<T>::executeInverse(idft_plan, complex_buffer, real_buffer);

    // Copy to the output, removing the pad
    auto wpad = (padded_width - image_ptr->getWidth()) / 2;
    auto hpad = (padded_height - image_ptr->getHeight()) / 2;
    for (int y = 0; y < image_ptr->getHeight(); ++y) {
      for (int x = 0; x < image_ptr->getWidth(); ++x) {
        image_ptr->setValue(x, y, real_buffer[x + wpad + (y + hpad) * padded_width] / total_size);
      }
    }
  }

  std::shared_ptr<const Image<T>> getKernel() const {
    return m_kernel;
  }

protected:
  void padMirroredKernel(int width, int height, typename std::vector<T>::iterator out) const {
    auto mirrored = MirrorImage<T>::create(m_kernel);
    auto padded = PaddedImage<T>::create(mirrored, width, height);
    auto center = PixelCoordinate{width / 2, height / 2};
    if (width % 2 == 0) center.m_x--;
    if (height % 2 == 0) center.m_y--;
    auto recenter = RecenterImage<T>::create(padded, center);

    dumpImage(recenter, out);
  }

  void dumpImage(const std::shared_ptr<const Image<T>> &img, typename std::vector<T>::iterator out) const {
    for (int y = 0; y < img->getHeight(); ++y) {
      for (int x = 0; x < img->getWidth(); ++x) {
        *out++ = img->getValue(x, y);
      }
    }
  }

private:
  std::shared_ptr<const Image<T>> m_kernel;
};

} // end SExtractor

#endif // _SEFRAMEWORK_CONVOLUTION_DFT_H
