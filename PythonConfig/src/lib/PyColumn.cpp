#include "PythonConfig/PyColumn.h"

namespace SExtractor {

int PyColumn::s_next_col_id;

PyColumn::PyColumn(): id(s_next_col_id++) {
}

PyColumn::~PyColumn() {
}

} // end SExtractor
