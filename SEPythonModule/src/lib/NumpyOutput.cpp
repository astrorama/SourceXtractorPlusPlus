/**
 * Copyright © 2022 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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

#include "SEPythonModule/NumpyOutput.h"
#include "Configuration/ConfigManager.h"
#include "Pyston/Table2Numpy.h"
#include "SEImplementation/Measurement/MeasurementFactory.h"
#include "SEPythonModule/PipelineReceiver.h"
#include "SEPythonModule/SourceInterface.h"
#include <boost/python/extract.hpp>
#include <boost/range/algorithm.hpp>

using Euclid::Table::Row;
using Euclid::Table::Table;
namespace py = boost::python;
namespace se = SourceXtractor;

namespace SourceXPy {

NumpyOutput::NumpyOutput(ContextPtr context) : m_context(std::move(context)) {
  m_source_to_row = m_context->m_source_to_row;
}

NumpyOutput::~NumpyOutput() {}

std::string NumpyOutput::repr() const {
  return "Output";
}

void NumpyOutput::receiveSource(std::unique_ptr<SourceXtractor::SourceGroupInterface> group) {
  boost::transform(*group, std::back_inserter(m_rows), m_source_to_row);
}

void NumpyOutput::receiveProcessSignal(const SourceXtractor::ProcessSourcesEvent& event) {
  auto all_done = std::dynamic_pointer_cast<AllFramesDone>(event.m_selection_criteria);
  if (all_done) {
    m_semaphore.release();
  }
}

void NumpyOutput::call(const py::object& obj) {
  py::extract<SourceGroup> group(obj);
  if (group.check()) {
    const auto&        group_ptr = group().m_group;
    Pyston::SaveThread save_thread;
    boost::transform(*group_ptr, std::back_inserter(m_rows), m_source_to_row);
    return;
  }
  py::extract<ProcessSourcesEvent> event_wrapper(obj);
  if (!event_wrapper.check()) {
    PyErr_SetString(PyExc_TypeError, "NumpyOutput: Unexpected python object received");
    py::throw_error_already_set();
  }
}

py::object NumpyOutput::getTable() {
  {
    Pyston::SaveThread save_thread;
    m_semaphore.acquire();
  }
  return Pyston::table2numpy(Table(m_rows));
}

}  // namespace SourceXPy
