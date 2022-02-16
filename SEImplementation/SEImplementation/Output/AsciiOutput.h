/** Copyright © 2019-2022 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
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
 * AsciiOutput.h
 *
 *  Created on: Feb 8, 2022
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_OUTPUT_ASCIIOUTPUT_H_
#define _SEIMPLEMENTATION_OUTPUT_ASCIIOUTPUT_H_

#include "Table/AsciiWriter.h"

#include "SEImplementation/Output/FlushableOutput.h"

namespace SourceXtractor {

class AsciiOutput : public FlushableOutput {

public:
  AsciiOutput (const std::string& filename, SourceToRowConverter source_to_row, size_t flush_size)
      : FlushableOutput(source_to_row, flush_size) {
    if (filename != "") {
      m_table_writer = std::make_shared<Euclid::Table::AsciiWriter>(filename);
    } else {
      m_table_writer = std::make_shared<Euclid::Table::AsciiWriter>(std::cout);
    }
  }

  void nextPart() override {
    // Do nothing
  }

protected:
  void writeRows(const std::vector<Euclid::Table::Row>& rows) override {
    Euclid::Table::Table table {rows};
    m_table_writer->addData(table);
  }

private:
  std::shared_ptr<Euclid::Table::TableWriter> m_table_writer;
};

}

#endif /* _SEIMPLEMENTATION_OUTPUT_ASCIIOUTPUT_H_ */
