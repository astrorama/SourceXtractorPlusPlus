/*
 * LdacOutput.h
 *
 *  Created on: Feb 10, 2022
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_OUTPUT_LDACOUTPUT_H_
#define _SEIMPLEMENTATION_OUTPUT_LDACOUTPUT_H_

#include "Table/FitsWriter.h"

#include "SEImplementation/Output/FlushableOutput.h"

namespace SourceXtractor {

class LdacOutput : public FlushableOutput {

public:
  LdacOutput (const std::string& filename, SourceToRowConverter source_to_row, size_t flush_size)
    : FlushableOutput(source_to_row, flush_size), m_filename(filename), m_part_nb(0), m_rms(0), m_gain(0) {
  }

  void nextPart() override;

  void outputSource(const SourceInterface& source) override;

protected:
  void writeRows(const std::vector<Euclid::Table::Row>& rows) override {
    Euclid::Table::Table table {rows};
    m_fits_writer->addData(table);
  }

private:
  void writeHeaders();

  std::string m_filename;
  int m_part_nb;

  std::shared_ptr<Euclid::Table::FitsWriter> m_fits_writer;

  std::map<std::string, MetadataEntry> m_image_metadata {};
  DetectionImage::PixelType m_rms;
  double m_gain;
};

}

#endif /* _SEIMPLEMENTATION_OUTPUT_LDACOUTPUT_H_ */
