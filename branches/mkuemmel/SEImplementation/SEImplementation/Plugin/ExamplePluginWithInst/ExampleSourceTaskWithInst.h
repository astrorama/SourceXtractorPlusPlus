#include "SEFramework/Task/SourceTask.h"
#include "SEImplementation/Property/PixelCoordinateList.h"
#include "SEImplementation/Plugin/DetectionFramePixelValues/DetectionFramePixelValues.h"
#include "SEFramework/Image/Image.h"
namespace SExtractor {
class ExampleSourceTaskWithInst : public SourceTask {
public:
  virtual ~ExampleSourceTaskWithInst() = default;
  ExampleSourceTaskWithInst(std::shared_ptr<FlagImage> flag_image, unsigned int flag_instance) :
    m_flag_image(flag_image), m_flag_instance(flag_instance) {
  }
  virtual void computeProperties(SourceInterface& source) const {
    double sum = 0.0;
    for (auto& coords : source.getProperty<PixelCoordinateList>().getCoordinateList()) {
      sum += m_flag_image->getValue(coords.m_x, coords.m_y);
    }
  source.setIndexedProperty<ExamplePropertyWithInst>(m_flag_instance, sum);
};
private:
  std::shared_ptr<FlagImage> m_flag_image;
  unsigned int m_flag_instance;
}; // End of ExampleSourceTaskWithInst class
} // namespace SExtractor
