#include "SEFramework/Property/Property.h"
namespace SExtractor {
class ExamplePropertyWithInst : public Property {
public:
  virtual ~ExamplePropertyWithInst() = default;
  ExamplePropertyWithInst(double flag_value_sum) : m_flag_value_sum(flag_value_sum) {}
  double getFlagValueSum() const {
    return m_flag_value_sum;
  }
private:
  double m_flag_value_sum;
}; // End of ExamplePropertyWithInst class
} // namespace SExtractor
