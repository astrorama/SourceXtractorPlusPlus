/*
 * @file SEFramework/Convolution/DFT.h
 * @date 17/09/18
 * @author aalvarez
 */

#ifndef _SEFRAMEWORK_CONVOLUTION_DFT_H
#define _SEFRAMEWORK_CONVOLUTION_DFT_H

#include "ModelFitting/ModelFitting/utils.h"
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

  struct ConvolutionContext {
  private:
    int m_padded_width, m_padded_height, m_total_size;
    std::vector<complex_t> m_kernel_transform, m_complex_buffer;
    std::vector<real_t> m_real_buffer;
    typename FFT<T>::plan_ptr_t m_fwd_plan, m_inv_plan;

    friend class DFTConvolution<T, TPadding>;
  };

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
  std::unique_ptr<ConvolutionContext> prepare(const std::shared_ptr<Image<T>> model_ptr) const {
    auto context = make_unique<ConvolutionContext>();

    // Dimension of the working padded images
    context->m_padded_width = model_ptr->getWidth() + m_kernel->getWidth() - 1;
    context->m_padded_height = model_ptr->getHeight() + m_kernel->getHeight() - 1;

    // For performance, use a size that is convenient for FFTW
    context->m_padded_width = fftRoundDimension(context->m_padded_width);
    context->m_padded_height = fftRoundDimension(context->m_padded_height);

    // Total number of pixels
    context->m_total_size = context->m_padded_width * context->m_padded_height;

    // Pre-allocate buffers for the transformations
    context->m_real_buffer.resize(context->m_total_size);
    context->m_complex_buffer.resize(context->m_total_size);
    context->m_kernel_transform.resize(context->m_total_size);

    // Since we already have the buffers, get the plans too
    context->m_fwd_plan = FFT<T>::createForwardPlan(1, context->m_padded_width, context->m_padded_height,
                                                    context->m_real_buffer, context->m_complex_buffer);
    context->m_inv_plan = FFT<T>::createInversePlan(1, context->m_padded_width, context->m_padded_height,
                                                    context->m_complex_buffer, context->m_real_buffer);

    // Transform here the kernel into frequency space
    padKernel(context->m_padded_width, context->m_padded_height, context->m_real_buffer.begin());
    FFT<T>::executeForward(context->m_fwd_plan, context->m_real_buffer, context->m_complex_buffer);
    std::copy(std::begin(context->m_complex_buffer), std::end(context->m_complex_buffer),
              std::begin(context->m_kernel_transform));

    return context;
  }

  template <typename ...Args>
  void convolve(std::shared_ptr<WriteableImage<T>> image_ptr,
                std::unique_ptr<ConvolutionContext>& context,
                Args... padding_args) const {
    // Padded image
    auto padded = TPadding::create(image_ptr,
                                   context->m_padded_width, context->m_padded_height,
                                   std::forward<Args>(padding_args)...);

    // Create a matrix with the padded image
    dumpImage(padded, context->m_real_buffer.begin());

    // Transform the image
    FFT<T>::executeForward(context->m_fwd_plan, context->m_real_buffer, context->m_complex_buffer);

    // Multiply the two DFT
    for (int i = 0; i < context->m_total_size; ++i) {
      context->m_complex_buffer[i] *= context->m_kernel_transform[i];
    }

    // Inverse DFT
    FFT<T>::executeInverse(context->m_inv_plan, context->m_complex_buffer, context->m_real_buffer);

    // Copy to the output, removing the pad
    auto wpad = (context->m_padded_width - image_ptr->getWidth()) / 2;
    auto hpad = (context->m_padded_height - image_ptr->getHeight()) / 2;
    for (int y = 0; y < image_ptr->getHeight(); ++y) {
      for (int x = 0; x < image_ptr->getWidth(); ++x) {
        image_ptr->setValue(x, y,
          context->m_real_buffer[x + wpad + (y + hpad) * context->m_padded_width] / context->m_total_size);
      }
    }
  }

  template <typename ...Args>
  void convolve(std::shared_ptr<WriteableImage<T>> image_ptr, Args... padding_args) const {
    auto context = prepare(image_ptr);
    convolve(image_ptr, context, std::forward(padding_args)...);
  }

  std::shared_ptr<const Image<T>> getKernel() const {
    return m_kernel;
  }

protected:
  void padKernel(int width, int height, typename std::vector<T>::iterator out) const {
    auto padded = PaddedImage<T>::create(m_kernel, width, height);
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
