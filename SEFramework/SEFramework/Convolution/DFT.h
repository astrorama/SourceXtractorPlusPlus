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
/*
 * @file SEFramework/Convolution/DFT.h
 * @date 17/09/18
 * @author aalvarez
 */

#ifndef _SEFRAMEWORK_CONVOLUTION_DFT_H
#define _SEFRAMEWORK_CONVOLUTION_DFT_H

#include "AlexandriaKernel/memory_tools.h"
#include "SEFramework/FFT/FFT.h"
#include "SEFramework/FFT/FFTHelper.h"
#include "SEFramework/Image/MirrorImage.h"
#include "SEFramework/Image/PaddedImage.h"
#include "SEFramework/Image/RecenterImage.h"
#include "SEFramework/Image/WriteableImage.h"

#include <fftw3.h>


namespace SourceXtractor {

/**
 * Convolution strategy based on the Discrete Fourier Transform
 * @tparam T
 *  The pixel type
 * @tparam TPadding
 *  The padding strategy
 */
template<typename T = SeFloat, class TPadding = PaddedImage<T, Reflect101Coordinates>>
class DFTConvolution {
public:
  typedef T real_t;
  typedef typename FFT<T>::complex_t complex_t;

  /**
   * This strategy has the concept of a "context": things that can be precomputed only once
   * and reused afterwards. This is useful, for instance, when the image is convolved multiple times
   * with the same kernel (i.e. ModelFitting)
   */
  struct ConvolutionContext {
    ConvolutionContext() = default;

  private:
    int m_padded_width, m_padded_height, m_transform_padding;
    std::vector<real_t> m_kernel_transform, m_work_area;
    typename FFT<T>::plan_ptr_t m_fwd_plan, m_inv_plan;

    friend class DFTConvolution<T, TPadding>;
  };

  /**
   * Constructor
   * @param img
   *    Convolution kernel
   */
  DFTConvolution(std::shared_ptr<const Image<T>> img)
    : m_kernel{std::move(img)} {
  }

  /**
   * Destructor
   */
  virtual ~DFTConvolution() = default;

  /**
   * @return
   *    The width of the kernel
   */
  std::size_t getWidth() const {
    return m_kernel->getWidth();
  }

  /**
   * @return
   *    The height of the kernel
   */
  std::size_t getHeight() const {
    return m_kernel->getHeight();
  }

  /**
   * Pre-computes the transform of the kernel, adapted to the image passed by model_ptr
   * @param model_ptr
   *    Reference image. Only its size is used to compute the necessary padding, and kernel size
   *    before transformation
   * @return
   *    A context than can be used by `convolve` to avoid re-computing the kernel multiple times
   */
  std::unique_ptr<ConvolutionContext> prepare(const std::shared_ptr<const Image<T>>& model_ptr) const {
    auto context = Euclid::make_unique<ConvolutionContext>();

    // Dimension of the working padded images
    context->m_padded_width = model_ptr->getWidth() + m_kernel->getWidth() - 1;
    context->m_padded_height = model_ptr->getHeight() + m_kernel->getHeight() - 1;

    // For performance, use a size that is convenient for FFTW
    context->m_padded_width = fftRoundDimension(context->m_padded_width);
    context->m_padded_height = fftRoundDimension(context->m_padded_height);

    // Need to add extra padding for storing the complex part too
    // (See FFTW documentation)
    context->m_transform_padding = 2 * (context->m_padded_width / 2 + 1) - context->m_padded_width;
    int work_area_size = context->m_padded_height * (context->m_padded_width / 2 + 1) * 2;

    // Pre-allocate buffers for the transformations
    context->m_kernel_transform.resize(work_area_size);
    context->m_work_area.resize(work_area_size);

    // Since we already have the buffers, get the plans too
    context->m_fwd_plan = FFT<T>::createForwardPlan(context->m_padded_width, context->m_padded_height,
                                                    context->m_work_area);
    context->m_inv_plan = FFT<T>::createInversePlan(context->m_padded_width, context->m_padded_height,
                                                    context->m_work_area);

    // Transform here the kernel into frequency space
    padKernel(*context);
    FFT<T>::executeForward(context->m_fwd_plan, context->m_kernel_transform);

    return context;
  }

  /**
   * Convolve the image with the stored kernel, using the given context storing the pre-computed
   * kernel transform, and pre-allocated buffers
   * @tparam Args
   *    Types of the parameters to be forwarded to the padding strategy
   * @param image_ptr
   *    The image to convolve
   * @param context
   *    The prepared context
   * @param padding_args
   *    Forwarded to the padding strategy
   */
  template <typename ...Args>
  void convolve(std::shared_ptr<WriteableImage<T>> image_ptr,
                std::unique_ptr<ConvolutionContext>& context,
                Args... padding_args) const {
    assert(image_ptr->getWidth() <= context->m_padded_width);
    assert(image_ptr->getHeight() <= context->m_padded_height);

    // Padded image
    auto padded = TPadding::create(image_ptr,
                                   context->m_padded_width, context->m_padded_height,
                                   std::forward<Args>(padding_args)...);

    // Create a matrix with the padded image
    dumpImage(padded, context->m_work_area);

    // Transform the image
    FFT<T>::executeForward(context->m_fwd_plan, context->m_work_area);

    // Multiply the two DFT
    complex_t* kernel_complex = reinterpret_cast<complex_t*>(context->m_kernel_transform.data());
    complex_t* img_complex    = reinterpret_cast<complex_t*>(context->m_work_area.data());
    size_t     ncomplex       = (context->m_padded_width / 2 + 1) * context->m_padded_height;
    for (size_t i = 0; i < ncomplex; ++i) {
      const auto& a  = img_complex[i];
      const auto& b  = kernel_complex[i];
      float       re = a[0] * b[0] - a[1] * b[1];
      float       im = a[0] * b[1] + a[1] * b[0];

      img_complex[i][0] = re;
      img_complex[i][1] = im;
    }

    // Inverse DFT
    FFT<T>::executeInverse(context->m_inv_plan, context->m_work_area);

    // Copy to the output, removing the pad
    auto wpad = ::div(context->m_padded_width - image_ptr->getWidth(), 2);
    auto lpad = wpad.quot;
    auto rpad = wpad.quot + wpad.rem;
    auto hpad = ::div(context->m_padded_height - image_ptr->getHeight(), 2);
    auto tpad = hpad.quot;
    auto bpad = hpad.quot + hpad.rem;
    copyFFTWorkAreaToImage(context->m_work_area, *image_ptr, rpad, lpad, tpad, bpad, true);
  }

  /**
   * Convolve the image with the stored kernel
   * @tparam Args
   *    Types of the parameters to be forwarded to the padding strategy
   * @param image_ptr
   *    The image to convolve
   * @param padding_args
   *    Forwarded to the padding strategy
   * @note
   *    The context will be computed on the fly, and discarded. If multiple, similar, convolutions are
   *    to be done, use a combination of prepare/convolve
   */
  template <typename ...Args>
  void convolve(std::shared_ptr<WriteableImage<T>> image_ptr, Args... padding_args) const {
    auto context = prepare(image_ptr);
    convolve(image_ptr, context, std::forward(padding_args)...);
  }

  /**
   * @return
   *    The convolution kernel
   */
  std::shared_ptr<const Image<T>> getKernel() const {
    return m_kernel;
  }

protected:
  void padKernel(ConvolutionContext& context) const {
    auto padded = PaddedImage<T>::create(m_kernel, context.m_padded_width, context.m_padded_height);
    auto center = PixelCoordinate{context.m_padded_width / 2, context.m_padded_height / 2};
    if (context.m_padded_width % 2 == 0) center.m_x--;
    if (context.m_padded_height % 2 == 0) center.m_y--;
    auto recenter = RecenterImage<T>::create(padded, center);

    dumpImage(recenter, context.m_kernel_transform);
  }

  void dumpImage(const std::shared_ptr<const Image<T>> &img, std::vector<T>& work_area) const {
    const auto chunk = img->getChunk(0, 0, img->getWidth(), img->getHeight());
    copyImageToFFTWorkArea(*chunk, work_area);
  }

private:
  std::shared_ptr<const Image<T>> m_kernel;
};

} // end SourceXtractor

#endif // _SEFRAMEWORK_CONVOLUTION_DFT_H
