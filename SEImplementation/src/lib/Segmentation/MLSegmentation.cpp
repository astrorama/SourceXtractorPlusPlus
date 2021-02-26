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

#include <memory>
#include <vector>
#include <list>
#include <iostream>

#include <onnxruntime_cxx_api.h>

#include "SEUtils/PixelCoordinate.h"
#include "SEUtils/HilbertCurve.h"

#include "SEFramework/Frame/Frame.h"

#include "SEFramework/FITS/TemporaryFitsImageSource.h"
#include "SEFramework/FITS/FitsWriter.h"

#include "SEFramework/Image/TileManager.h"
#include "SEFramework/Image/VectorImage.h"

#include "SEImplementation/Common/OnnxCommon.h"

#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Property/SourceId.h"

#include "SEImplementation/Measurement/MultithreadedMeasurement.h"


#include "SEImplementation/Grouping/LineSelectionCriteria.h"
#include "SEImplementation/Segmentation/Lutz.h"

#include "SEImplementation/Common/OnnxModel.h"
#include "SEImplementation/CheckImages/CheckImages.h"

#include "SEImplementation/Segmentation/MLSegmentation.h"



namespace SourceXtractor {

namespace {
class LutzLabellingListener : public Lutz::LutzListener {
public:
  LutzLabellingListener(Segmentation::LabellingListener& listener, std::shared_ptr<SourceFactory> source_factory,
      int window_size) :
    m_listener(listener),
    m_source_factory(source_factory),
    m_window_size(window_size) {}

  virtual ~LutzLabellingListener() = default;

  void publishGroup(Lutz::PixelGroup& pixel_group) override {
    auto source = m_source_factory->createSource();
    source->setProperty<PixelCoordinateList>(pixel_group.pixel_list);
    source->setProperty<SourceId>();
    m_listener.publishSource(source);
  }

  void notifyProgress(int line, int total) override {
    m_listener.notifyProgress(line, total);

    if (m_window_size > 0 && line > m_window_size) {
      m_listener.requestProcessing(
        ProcessSourcesEvent(std::make_shared<LineSelectionCriteria>(line - m_window_size))
      );
    }
  }

private:
  Segmentation::LabellingListener& m_listener;
  std::shared_ptr<SourceFactory> m_source_factory;
  int m_window_size;
};


}

void MLSegmentation::labelImage(Segmentation::LabellingListener& listener, std::shared_ptr<const DetectionImageFrame> frame) {
  Elements::Logging onnx_logger = Elements::Logging::getLogger("Onnx");

  auto allocator = Ort::AllocatorWithDefaultOptions();

  int tile_size = 400; // FIXME
  OnnxModel model(m_model_path);

  if (model.getInputType() != ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT) {
    throw Elements::Exception() << "Only ONNX models with float input are supported";
  }

  if (model.getOutputType() != ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT) {
    throw Elements::Exception() << "Only ONNX models with float output are supported";
  }

  // allocate memory
  std::vector<float> input_data(tile_size * tile_size);
  std::vector<float> output_data(tile_size * tile_size * 3);

  //auto var_image = frame->getVarianceMap();
  //auto processed_image = VectorImage<float>::create(tile_size, tile_size);

  auto image = frame->getSubtractedImage();
  auto check = CheckImages::getInstance().getMoffatImage();
  auto tmp_image = FitsWriter::newTemporaryImage<float>("_tmp_ml_seg%%%%%%.fits", image->getWidth(), image->getHeight());

  Lutz lutz;
  LutzLabellingListener lutz_listener(listener, m_source_factory, 0);

  for (int ox = 0; ox + tile_size * 3 / 4 < image->getWidth(); ox += tile_size / 2) {
    for (int oy = 0; oy + tile_size * 3 / 4 < image->getHeight(); oy += tile_size / 2) {

      for (int x = 0; x < tile_size; x++) {
        for (int y = 0; y < tile_size; y++) {
          if (ox+x < image->getWidth() && oy+y < image->getHeight()) {
            input_data[x+y*tile_size] = image->getValue(ox+x, oy+y) / 20.1686; // FIXME get average RMS
          } else {
            input_data[x+y*tile_size] = 0;
          }
        }
      }

      model.run<float, float>(input_data, output_data);

      int start_x = (ox == 0) ? 0 : tile_size / 4;
      int start_y = (oy == 0) ? 0 : tile_size / 4;

      int end_x = (ox + tile_size * 5 / 4 < image->getWidth()) ? tile_size * 3 / 4 : tile_size ;
      int end_y = (oy + tile_size * 5 / 4 < image->getHeight()) ? tile_size * 3 / 4 : tile_size;

      for (int x = start_x; x < end_x; x++) {
        for (int y = start_y; y < end_y; y++) {
          if (ox+x < image->getWidth() && oy+y < image->getHeight()) {
//            float a = output_data[(x+y*tile_size) * 3 + 0];
            float b = output_data[(x+y*tile_size) * 3 + 1];
////            float c = output_data[(x+y*tile_size) * 3 + 2];
//            //std::cout << a << ": " << b << " " << c <<"\n";
            tmp_image->setValue(ox + x, oy + y,  b - 0.7f);

//            float a = output_data[(x+y*tile_size)];
//            processed_image->setValue(x, y,  b);
            if (check != nullptr) {
              check->setValue(ox+x, oy+y, b);
            }
          }
        }
      }
    }
  }
  lutz.labelImage(lutz_listener, *tmp_image);
}

}
