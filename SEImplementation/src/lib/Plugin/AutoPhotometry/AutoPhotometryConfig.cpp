/*
 * AperturePhotometryConfig.cpp
 *
 *  Created on: Jul 18, 2018
 *      Author: mkuemmel@usm.lmu.de
 */

#include "SEUtils/Types.h"

#include "Configuration/ProgramOptionsHelper.h"
#include "SEImplementation/Plugin/AutoPhotometry/AutoPhotometryConfig.h"

namespace po = boost::program_options;
using poh = Euclid::Configuration::ProgramOptionsHelper;

namespace SExtractor {

namespace {
const std::string AUTO_KRON_FACTOR {"auto-kron-factor"};
const std::string AUTO_KRON_MINRAD {"auto-kron-minrad"};
const std::string AUTO_KRON_ESTIMATION  {"auto-kron-estimation"};
const std::string AUTO_KRON_MEASUREMENT {"auto-kron-measurement"};
}

auto AutoPhotometryConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Auto (Kron) photometry options", {
      {AUTO_KRON_FACTOR.c_str(), po::value<double>()->default_value(2.5), "Scale factor for AUTO (Kron) photometry"},
      {AUTO_KRON_MINRAD.c_str(), po::value<double>()->default_value(3.5), "Minimum radius for AUTO (Kron) photometry"},
      {AUTO_KRON_ESTIMATION.c_str(), po::value<double>()->default_value(0.0), "First value of PHOT_AUTOAPERS <estimation>"},
      {AUTO_KRON_MEASUREMENT.c_str(), po::value<double>()->default_value(0.0), "Second value of PHOT_AUTOAPERS <measurement>"}
  }}};
}

void AutoPhotometryConfig::initialize(const UserValues& args) {
  m_kron_factor = SeFloat(args.at(AUTO_KRON_FACTOR).as<double>());;
  m_kron_minrad = SeFloat(args.at(AUTO_KRON_MINRAD).as<double>());;
  m_kron_estimation = SeFloat(args.at(AUTO_KRON_ESTIMATION).as<double>());;
  m_kron_measurement = SeFloat(args.at(AUTO_KRON_MEASUREMENT).as<double>());;
}

SeFloat AutoPhotometryConfig::getAutoKronFactor() const {
  return m_kron_factor;
}
SeFloat AutoPhotometryConfig::getAutoKronMinrad() const {
  return m_kron_minrad;
}
SeFloat AutoPhotometryConfig::getAutoKronEstimation() const {
  return m_kron_estimation;
}
SeFloat AutoPhotometryConfig::getAutoKronMeasurement() const {
  return m_kron_measurement;
}

} // SExtractor namespace
