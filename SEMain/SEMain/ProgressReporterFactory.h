/** Copyright © 2019 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
/*
 * ProgressReporterFactory.h
 *
 *  Created on: Mar 19, 2019
 *      Author: Alejandro Alvarez Ayllon
 */

#ifndef _SEMAIN_PROGRESSPRINTERFACTORY_H
#define _SEMAIN_PROGRESSPRINTERFACTORY_H

#include <chrono>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include "SEMain/ProgressMediator.h"

namespace SourceXtractor {

/**
 * @class ProgressReporterFactory
 *  Abstracts away the creation of concrete ProgressReporters
 * @note
 *  This class is not a Configurable because it needs to be started before the rest of the
 *  configurables. Otherwise, their logging can not be intercepted.
 */
class ProgressReporterFactory {
public:
  virtual ~ProgressReporterFactory() = default;

  ProgressReporterFactory();

  void addOptions(boost::program_options::options_description &options) const;

  void configure(const std::map<std::string, boost::program_options::variable_value> &args);

  std::shared_ptr<ProgressMediator> createProgressMediator(void) const;

private:
  std::chrono::steady_clock::duration m_min_interval;
  bool m_disable_progress_bar;
};

}

#endif // _SEMAIN_PROGRESSPRINTERFACTORY_H
