/*
 * OnnxCompactModel.h
 *
 *  Created on: Sep 3, 2021
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_ONNXCOMPACTMODEL_H_
#define _SEIMPLEMENTATION_PLUGIN_FLEXIBLEMODELFITTING_ONNXCOMPACTMODEL_H_

#include <numeric>

#include "ModelFitting/Models/CompactModelBase.h"

#include "SEImplementation/Common/OnnxModel.h"
#include "SEImplementation/Common/OnnxCommon.h"

namespace ModelFitting {

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

  double getValue(double x, double y) const override {
    // FIXME TBI
    return 0.0;
  }

  ImageType getRasterizedImage(double pixel_scale, std::size_t size_x, std::size_t size_y) const override {
    using Traits = ImageTraits<ImageType>;
    ImageType image = Traits::factory(size_x, size_y);


    std::size_t largest_size = std::max(size_x, size_y);

    std::shared_ptr<SourceXtractor::OnnxModel> selected_model;
    for (auto model : m_models) {
      auto shape = model->getOutputShape();
      if (largest_size < shape[2]) {
        selected_model = model;
        break;
      }
    }

    //std::cout << largest_size << " / " << selected_model->getModelPath() << "\n";

    auto input_shape = selected_model->getInputShape();
    auto output_shape = selected_model->getOutputShape();
    int render_size = output_shape[2];

    // TODO add sanity check
    int tile_size = output_shape[1];
    int data_planes = output_shape[3];

//    onnx_logger.info() << "Onnx tile size: " << tile_size << " Data planes: " << data_planes << " RMS: " << average_rms;
//
//    if (model.getInputType() != ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT) {
//      throw Elements::Exception() << "Only ONNX models with float input are supported";
//    }
//
//    if (model.getOutputType() != ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT) {
//      throw Elements::Exception() << "Only ONNX models with float output are supported";
//    }

    // allocate memory
    std::map<std::string, std::vector<float>> input_data_arrays;
    std::vector<float> output_data(tile_size * tile_size * data_planes);

    for (auto const& it : m_params) {
//      std::cout << ">" << it.first << " " << (it.second->getValue()) << "\n";
      input_data_arrays[it.first] = std::vector<float>( { it.second->getValue() } );
    }

    input_data_arrays["x"] = std::vector<float>(render_size * render_size);
    input_data_arrays["y"] = std::vector<float>(render_size * render_size);

    auto transform = getCombinedTransform(pixel_scale);

    for (int x=0; x<(int)size_x; ++x) {
      int dx = x - size_x / 2;
      for (int y=0; y<(int)size_y; ++y) {
        int dy = y - size_y / 2;

        float x2 = dx * transform[0] + dy * transform[1];
        float y2 = dx * transform[2] + dy * transform[3];

        input_data_arrays["x"][x + y * render_size] = x2;
        input_data_arrays["y"][x + y * render_size] = y2;
      }
    }

    //fixme use correct model
    selected_model->runMultiInput<float, float>(input_data_arrays, output_data);
    // fixme size mismatch!


//    double total_flux = std::accumulate(output_data.begin(), output_data.end(), 0.f);
//    //double area_correction = (1.0 / fabs(m_jacobian[0] * m_jacobian[3] - m_jacobian[1] * m_jacobian[2])) * pixel_scale * pixel_scale;
//    double correction = (m_flux->getValue() / total_flux);

//    std::cout << "TF " << total_flux << "\n";

    for (int x = 0; x < (int) size_x; ++x) {
      for (int y = 0; y < (int) size_y; ++y) {
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
