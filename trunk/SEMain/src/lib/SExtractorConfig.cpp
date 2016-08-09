/**
 * @file src/lib/SExtractorConfig.cpp
 * @date 07/20/16
 * @author nikoapos
 */

#include "SEMain/SExtractorConfig.h"
#include "SEImplementation/Configuration/BackgroundConfig.h"
#include "SEImplementation/Configuration/MinAreaPartitionConfig.h"
#include "SEImplementation/Configuration/AttractorsPartitionConfig.h"

namespace po = boost::program_options;

namespace SExtractor {

static const std::string LIST_OUTPUT_COLUMNS {"list-columns"};

SExtractorConfig::SExtractorConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<BackgroundConfig>();
  declareDependency<MinAreaPartitionConfig>();
  declareDependency<AttractorsPartitionConfig>();
}

auto SExtractorConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return { {"", {
      {LIST_OUTPUT_COLUMNS.c_str(), po::bool_switch(),
          "List the possible output columns for the given input parameters and exit"}
  }}};
}

void SExtractorConfig::initialize(const UserValues& args) {
  m_list_columns_flag = args.at(LIST_OUTPUT_COLUMNS).as<bool>();
}

bool SExtractorConfig::listOutputColumns() const {
  return m_list_columns_flag;
}

} // SExtractor namespace



