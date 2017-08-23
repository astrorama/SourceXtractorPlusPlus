#include "SEFramework/Task/SourceTask.h"
#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValues.h"
namespace SExtractor {
class ExampleSourceTaskWithConf: public SourceTask {
public:
  virtual ~ExampleSourceTaskWithConf () = default;
  ExampleSourceTaskWithConf (std::string a_name, double a_double_value) :
    m_name_option(a_name), m_scaling_factor_option(a_double_value) {
  }
  virtual void computeProperties (SourceInterface& source) const {
    const auto& pixel_values = source.getProperty<DetectionFramePixelValues>().getValues();
    double mean_value = 0.0;
    for (auto value : pixel_values) {
      mean_value += value;
    }
    mean_value /= pixel_values.size();
    source.setProperty<ExamplePropertyWithConf>(m_name_option, m_scaling_factor_option * mean_value);
  }
private:
  std::string m_name_option;
  double m_scaling_factor_option;
}; // End of ExampleSourceTaskWithConf class
} // namespace SExtractor
