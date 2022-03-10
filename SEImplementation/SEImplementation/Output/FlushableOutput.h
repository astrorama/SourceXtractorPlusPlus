/*
 * FlushableOutput.h
 *
 *  Created on: Feb 8, 2022
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_OUTPUT_FLUSHABLEOUTPUT_H_
#define _SEIMPLEMENTATION_OUTPUT_FLUSHABLEOUTPUT_H_

#include "Table/Row.h"

#include "SEFramework/Output/Output.h"

namespace SourceXtractor {

class FlushableOutput : public Output {

public:
  using SourceToRowConverter = std::function<Euclid::Table::Row(const SourceInterface&)>;

  FlushableOutput(SourceToRowConverter source_to_row, size_t flush_size)
      : m_source_to_row(source_to_row), m_flush_size(flush_size), m_total_rows_written(0) {
  }

  virtual ~FlushableOutput() = default;

  size_t flush() override {
    if (!m_rows.empty()) {
      writeRows(m_rows);
    }
    m_total_rows_written += m_rows.size();
    m_rows.clear();
    return m_total_rows_written;
  }

  void outputSource(const SourceInterface& source) override {
    m_rows.emplace_back(m_source_to_row(source));
    if (m_flush_size > 0 && m_rows.size() % m_flush_size == 0) {
      flush();
    }
  }

protected:
  virtual void writeRows(const std::vector<Euclid::Table::Row>& rows) = 0;

private:
  SourceToRowConverter m_source_to_row;
  size_t m_flush_size;

  std::vector<Euclid::Table::Row> m_rows {};
  size_t m_total_rows_written;
};

}

#endif /* SEIMPLEMENTATION_SEIMPLEMENTATION_OUTPUT_FLUSHABLEOUTPUT_H_ */
