#include "SEFramework/Property/Property.h"
namespace SExtractor {
class ExampleProperty : public Property {
public:
  virtual ~ExampleProperty() = default;
  ExampleProperty(double pixel_mean_value) : m_pixel_mean_value(pixel_mean_value) {}
  double getPixelMeanValue () const {
    return m_pixel_mean_value;
  }
private:
  double m_pixel_mean_value;
}; // End of ExampleProperty class
} // namespace SExtractor
