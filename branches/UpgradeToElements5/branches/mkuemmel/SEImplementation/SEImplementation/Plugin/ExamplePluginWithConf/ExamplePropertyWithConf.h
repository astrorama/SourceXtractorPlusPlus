#include "SEFramework/Property/Property.h"
namespace SExtractor {
class ExamplePropertyWithConf : public Property {
public:
  virtual ~ExamplePropertyWithConf() = default;
  ExamplePropertyWithConf(std::string name, double value) : m_name(name), m_scaled_mean(value) {}
  const std::string& getName () const {
    return m_name;
  }
  double getScaledMean () const {
    return m_scaled_mean;
  }
private:
  std::string m_name;
  double m_scaled_mean;
}; // End of ExamplePropertyWithConf class
} // namespace SExtractor
