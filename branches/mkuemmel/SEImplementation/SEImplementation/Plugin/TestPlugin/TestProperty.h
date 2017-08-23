#include "SEFramework/Property/Property.h"
namespace SExtractor {
class TestProperty : public Property {
public:
  virtual ~TestProperty() = default;
  TestProperty(int ivalue) : m_int(ivalue){}
  //const std::string& getName () const {
  //  return m_name;
  //}
  double getTestInt () const {
    return m_int;
  }
private:
  //std::string m_name;
  int m_int;
}; // End of ExamplePropertyWithConf class
} // namespace SExtractor
