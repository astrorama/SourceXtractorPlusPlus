/*
 * TextOutput.h_
 *
 *  Created on: Jun 24, 2016
 *      Author: mschefer
 */

#ifndef _SEIMPLEMENTATION_OUTPUT_TEXTOUTPUT_H_
#define _SEIMPLEMENTATION_OUTPUT_TEXTOUTPUT_H_

#include "SEFramework/Output/OutputColumn.h"
#include "SEFramework/Output/Output.h"

namespace SExtractor {

class TextOutput : public Output {
public:
  virtual ~TextOutput() = default;

  TextOutput(std::vector<OutputColumn> output_columns);

  virtual void outputSource(const SourceInterface& source) override;

private:
  std::vector<OutputColumn> m_output_columns;
};


} // namespace SExtractor

#endif /* _SEIMPLEMENTATION_OUTPUT_TEXTOUTPUT_H_ */
