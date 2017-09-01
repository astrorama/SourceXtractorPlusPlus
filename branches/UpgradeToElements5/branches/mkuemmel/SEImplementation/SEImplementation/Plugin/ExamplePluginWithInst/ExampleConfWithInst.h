#include "Configuration/Configuration.h"
#include "SEFramework/Image/Image.h"
namespace SExtractor {
class ExampleConfWithInst : public Euclid::Configuration::Configuration {
public:
  virtual ~ExampleConfWithInst() = default;
  ExampleConfWithInst(long manager_id) : Configuration(manager_id) {}
  std::map<std::string, OptionDescriptionList> getProgramOptions() override;
  void preInitialize(const UserValues& args) override;
  void initialize(const UserValues& args) override;
  const std::map<std::string, std::shared_ptr<FlagImage> >& getFlagImageMap () const {
    return m_flag_image_map;
  }
private:
  std::map<std::string, std::shared_ptr<FlagImage>> m_flag_image_map;
}; // End of ExampleConfWithInst class
} // namespace SExtractor
