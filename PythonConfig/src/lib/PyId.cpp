#include "PythonConfig/PyId.h"

namespace SExtractor {

int PyId::s_next_col_id;

PyId::PyId(): id(s_next_col_id++) {
}

PyId::~PyId() {
}

} // end SExtractor
