#include "SEFramework/Task/SourceTask.h"
#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValues.h"
namespace SExtractor {
class ExampleSourceTask : public SourceTask {
public:
  virtual ~ExampleSourceTask() = default;
  virtual void computeProperties(SourceInterface& source) const {
  const auto& pixel_values = source.getProperty<DetectionFramePixelValues>().getValues();
  double mean_value = 0.0;
  for (auto value : pixel_values) {
    mean_value += value;
  }
  mean_value /= pixel_values.size();

  source.setProperty<ExampleProperty>(mean_value);
};
private:
}; // End of ExampleSourceTask class
} // namespace SExtractor
