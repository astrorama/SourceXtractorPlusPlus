/*
 * FitsOutput.h
 *
 *  Created on: Feb 8, 2022
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_OUTPUT_FITSOUTPUT_H_
#define _SEIMPLEMENTATION_OUTPUT_FITSOUTPUT_H_

#include "Table/FitsWriter.h"

#include "SEImplementation/Output/FlushableOutput.h"

namespace SourceXtractor {

class FitsOutput : public FlushableOutput {

public:
  FitsOutput (const std::string& filename, SourceToRowConverter source_to_row, size_t flush_size)
    : FlushableOutput(source_to_row, flush_size), m_filename(filename), m_part_nb(0) {
    m_fits_writer = std::make_shared<Euclid::Table::FitsWriter>(m_filename, true);
    m_fits_writer->setHduName("CATALOG");
  }

  void nextPart() override {
    m_part_nb++;
    std::stringstream hdu_name;
    hdu_name << "CATALOG_" << m_part_nb;

    m_fits_writer = std::make_shared<Euclid::Table::FitsWriter>(m_filename);
    m_fits_writer->setHduName(hdu_name.str());
  }

protected:
  void writeRows(const std::vector<Euclid::Table::Row>& rows) override {
    Euclid::Table::Table table {rows};
    m_fits_writer->addData(table);
  }

private:
  std::string m_filename;
  int m_part_nb;

  std::shared_ptr<Euclid::Table::FitsWriter> m_fits_writer;
};

}

#endif /* _SEIMPLEMENTATION_OUTPUT_FITSOUTPUT_H_ */
