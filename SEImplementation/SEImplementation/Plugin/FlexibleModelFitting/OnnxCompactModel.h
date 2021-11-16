/*
 * OnnxCompactModel.h
 *
 *  Created on: Sep 3, 2021
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_ONNXCOMPACTMODEL_H_
#define _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_ONNXCOMPACTMODEL_H_

#include <numeric>

#include <ElementsKernel/Logging.h>

#include "ModelFitting/Models/CompactModelBase.h"

#include "SEImplementation/Common/OnnxModel.h"
#include "SEImplementation/Common/OnnxCommon.h"

namespace ModelFitting {

static auto logger = Elements::Logging::getLogger("FlexibleModelFitting");

template <typename ImageType>
class OnnxCompactModel : public CompactModelBase<ImageType> {
public:
  OnnxCompactModel(std::vector<std::shared_ptr<SourceXtractor::OnnxModel>> models,
                std::shared_ptr<BasicParameter> x_scale, std::shared_ptr<BasicParameter> y_scale,
                std::shared_ptr<BasicParameter> rotation, double width, double height,
                std::shared_ptr<BasicParameter> x, std::shared_ptr<BasicParameter> y,
                std::shared_ptr<BasicParameter> flux,
                std::map<std::string, std::shared_ptr<BasicParameter>> params,
                std::tuple<double, double, double, double> transform)
          : CompactModelBase<ImageType>(x_scale, y_scale, rotation, width, height, x, y, transform),
            m_models(models), m_flux(flux), m_params(params)
    {
    }

  virtual ~OnnxCompactModel() = default;

  double getValue(double, double) const override {
    return 0.0; // unused
  }

  ImageType getRasterizedImage(double pixel_scale, std::size_t size_x, std::size_t size_y) const override {
    using Traits = ImageTraits<ImageType>;
    ImageType image = Traits::factory(size_x, size_y);

    int largest_size = std::max(size_x, size_y);

    std::shared_ptr<SourceXtractor::OnnxModel> selected_model;
    for (auto model : m_models) {
      auto shape = model->getOutputShape();
      if (largest_size < shape[2]) {
        selected_model = model;
        break;
      }
    }

    if (selected_model == nullptr) {
      logger.warn() << "No large enough ONNX model could be found, skipping...";
      return image;
    }

    auto input_shape = selected_model->getInputShape();
    auto output_shape = selected_model->getOutputShape();
    int render_size = output_shape[2];

    if (selected_model->getOutputType() != ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT) {
      throw Elements::Exception() << "Only ONNX models with float output are supported";
    }

    // allocate memory
    std::map<std::string, std::vector<float>> input_data_arrays;
    std::vector<float> output_data(render_size * render_size);

    for (auto const& it : m_params) {
      input_data_arrays[it.first] = std::vector<float>( { it.second->getValue() } );
    }

    input_data_arrays["x"] = std::vector<float>(render_size * render_size);
    input_data_arrays["y"] = std::vector<float>(render_size * render_size);

    auto transform = getCombinedTransform(pixel_scale);

    for (int y=0; y<(int)size_y; ++y) {
      int dy = y - size_y / 2;
      for (int x=0; x<(int)size_x; ++x) {
        int dx = x - size_x / 2;

        float x2 = dx * transform[0] + dy * transform[1];
        float y2 = dx * transform[2] + dy * transform[3];

        input_data_arrays["x"][x + y * render_size] = x2;
        input_data_arrays["y"][x + y * render_size] = y2;
      }
    }

    selected_model->runMultiInput<float, float>(input_data_arrays, output_data);

    for (int y = 0; y < (int) size_y; ++y) {
      for (int x = 0; x < (int) size_x; ++x) {
          Traits::at(image, x, y) = output_data[x + y * render_size];
      }
    }

    renormalize(image, m_flux->getValue());
    return image;
  }

private:
  using CompactModelBase<ImageType>::getMaxRadiusSqr;
  using CompactModelBase<ImageType>::getCombinedTransform;
  using CompactModelBase<ImageType>::m_jacobian;
  using CompactModelBase<ImageType>::samplePixel;
  using CompactModelBase<ImageType>::adaptiveSamplePixel;
  using CompactModelBase<ImageType>::renormalize;

  std::vector<std::shared_ptr<SourceXtractor::OnnxModel>> m_models;

  // parameters
  std::shared_ptr<BasicParameter> m_flux;
  std::map<std::string, std::shared_ptr<BasicParameter>> m_params;
};

}



#endif /* _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_ONNXCOMPACTMODEL_H_ */
