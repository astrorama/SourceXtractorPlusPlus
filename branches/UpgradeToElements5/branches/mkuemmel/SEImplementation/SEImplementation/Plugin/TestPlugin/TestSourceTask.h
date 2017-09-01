#include "SEFramework/Task/SourceTask.h"
//#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValues.h"
namespace SExtractor {
class TestSourceTask: public SourceTask {
public:
  virtual ~TestSourceTask () = default;
  TestSourceTask (int a_int_value) : m_test_int_option(a_int_value) {
  }
  virtual void computeProperties (SourceInterface& source) const {
    //const auto& pixel_values = source.getProperty<DetectionFramePixelValues>().getValues();
    //double mean_value = 0.0;
    //for (auto value : pixel_values) {
    //  mean_value += value;
    //}
    //mean_value /= pixel_values.size();
    //source.setProperty<TestProperty>(m_name_option, m_scaling_factor_option * mean_value);
    source.setProperty<TestProperty>(m_test_int_option);
  }
private:
  //std::string m_name_option;
  int m_test_int_option;
}; // End of ExampleSourceTaskWithConf class
} // namespace SExtractor
