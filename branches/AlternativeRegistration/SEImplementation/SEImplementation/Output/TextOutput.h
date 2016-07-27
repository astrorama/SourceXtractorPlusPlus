/*
 * TextOutput.h_
 *
 *  Created on: Jun 24, 2016
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_OUTPUT_TEXTOUTPUT_H_
#define _SEIMPLEMENTATION_OUTPUT_TEXTOUTPUT_H_

#include <iostream>

#include "SEFramework/Output/OutputColumn.h"
#include "SEFramework/Output/Output.h"

namespace SExtractor {

class TextOutput : public Output {
public:
  virtual ~TextOutput() = default;

  TextOutput(std::vector<OutputColumn> output_columns, std::ostream& output_stream);

  virtual void outputSource(const SourceInterface& source) override;

private:
  std::vector<OutputColumn> m_output_columns;
  std::ostream& m_output_stream;
};


} // namespace SExtractor

#endif /* _SEIMPLEMENTATION_OUTPUT_TEXTOUTPUT_H_ */
