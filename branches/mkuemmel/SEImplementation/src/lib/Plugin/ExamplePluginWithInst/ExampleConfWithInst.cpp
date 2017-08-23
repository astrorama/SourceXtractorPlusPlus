#include <boost/filesystem.hpp>
#include "SEFramework/Image/FitsReader.h"
#include "SEImplementation/Plugin/ExamplePluginWithInst/ExampleConfWithInst.h"
namespace po = boost::program_options;
namespace fs = boost::filesystem;
namespace SExtractor {
namespace {
const std::string FLAG_IMAGE_FILES { "ex-flag-images-files" };
const std::string FLAG_IMAGE_IDS { "ex-flag-images-ids" };
}
auto ExampleConfWithInst::getProgramOptions () -> std::map<std::string, OptionDescriptionList> {
  return { {"Flag options", {
        { FLAG_IMAGE_FILES.c_str(), po::value<std::vector<std::string>>()->multitoken(),
          "The FITS files containing flags"},
        { FLAG_IMAGE_IDS.c_str(), po::value<std::vector<std::string>>()->multitoken(),
          "Identifiers of the flag images"},
      }}};
}
void ExampleConfWithInst::preInitialize (const UserValues& args) {
  if (args.count(FLAG_IMAGE_FILES) != 0) {
    auto filenames = args.find(FLAG_IMAGE_FILES)->second.as<std::vector<std::string>>();
    auto ids = args.find(FLAG_IMAGE_IDS)->second.as<std::vector<std::string>>();
    if (filenames.size() != ids.size()) {
      throw Elements::Exception() << "The number of flag IDs do not match the number of files !";
    }
    for (auto filename : filenames) {
      if (!fs::exists(filename)) {
        throw Elements::Exception() << "File " << filename << " does not exist";
      }
    }
  }
}
void ExampleConfWithInst::initialize (const UserValues& args) {
  if (args.count(FLAG_IMAGE_FILES) != 0) {
    auto& filenames = args.find(FLAG_IMAGE_FILES)->second.as<std::vector<std::string>>();
    auto& ids = args.find(FLAG_IMAGE_IDS)->second.as<std::vector<std::string>>();
    for (auto index = 0; index < filenames.size(); index++) {
      m_flag_image_map[ids.at(index)] = std::move(FitsReader<std::int64_t>::readFile(filenames.at(index)));
    }
  }
}
} // SExtractor namespace
