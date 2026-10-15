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
 * AsdfFile.cpp
 *
 *  Created on: Aug 14, 2025
 *      Author: embray
 */

#include <assert.h>

#include <cstdio>
#include <mutex>
#include <string>

#include <boost/algorithm/string/trim.hpp>
#include <boost/filesystem/path.hpp>

#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"

#include "SEFramework/ASDF/AsdfFile.h"

namespace SourceXtractor {

static Elements::Logging logger = Elements::Logging::getLogger("AsdfFile");

namespace {

/// libasdf's own messages, so they are attributable in the log
Elements::Logging asdf_logger = Elements::Logging::getLogger("libasdf");

/// Guards the partial-line buffer below; ASDF files may be read from worker threads
std::mutex log_mutex;

/**
 * Re-emit one complete libasdf log line through the Elements logger
 *
 * libasdf's formatter is fixed, so the level has to be recovered from the line it
 * produces.  With colour off and the package field dropped that is
 * "[LEVEL] file:line: message", where LEVEL is padded to five characters.
 *
 * Future libasdf versions will have better structured logging support, at
 * which point this workaround will be unnecessary and obsolete.
 */
void emitAsdfLogLine(const std::string& line) {
  std::string message = line;
  std::string level;

  if (!message.empty() && message.front() == '[') {
    auto close = message.find(']');
    if (close != std::string::npos) {
      level = boost::trim_copy(message.substr(1, close - 1));
      message = boost::trim_left_copy(message.substr(close + 1));
    }
  }

  if (message.empty()) {
    return;
  }

  // An unrecognized level is reported rather than dropped: losing a diagnostic is
  // worse than logging one at the wrong level
  if (level == "TRACE" || level == "DEBUG") {
    asdf_logger.debug() << message;
  } else if (level == "INFO") {
    asdf_logger.info() << message;
  } else if (level == "WARN") {
    asdf_logger.warn() << message;
  } else if (level == "FATAL") {
    asdf_logger.fatal() << message;
  } else {
    asdf_logger.error() << message;
  }
}


/// Accumulate bytes written by libasdf and emit each complete line
void consumeAsdfLogBytes(const char* data, size_t size) {
  static std::string pending;
  std::lock_guard<std::mutex> guard{log_mutex};

  for (size_t i = 0; i < size; i++) {
    if (data[i] == '\n') {
      emitAsdfLogLine(pending);
      pending.clear();
    } else {
      pending.push_back(data[i]);
    }
  }
}


#if defined(__GLIBC__)

ssize_t asdfLogCookieWrite(void* /* cookie */, const char* buf, size_t size) {
  consumeAsdfLogBytes(buf, size);
  return static_cast<ssize_t>(size);
}

FILE* openAsdfLogStream() {
  cookie_io_functions_t funcs{};
  funcs.write = &asdfLogCookieWrite;
  return fopencookie(nullptr, "w", funcs);
}

#elif defined(__APPLE__) || defined(BSD) || defined(__FreeBSD__)

int asdfLogFunopenWrite(void* /* cookie */, const char* buf, int size) {
  if (size > 0) {
    consumeAsdfLogBytes(buf, static_cast<size_t>(size));
  }
  return size;
}

FILE* openAsdfLogStream() {
  return funopen(nullptr, nullptr, &asdfLogFunopenWrite, nullptr, nullptr);
}

#else

/// No way to adapt a FILE* to a callback here; fall back to uncoloured stderr
FILE* openAsdfLogStream() {
  return nullptr;
}

#endif


/**
 * The stream libasdf writes its log messages to
 *
 * Lives for the lifetime of the process, so one stream serves every file.
 */
FILE* getAsdfLogStream() {
  static FILE* stream = []() -> FILE* {
    FILE* s = openAsdfLogStream();

    if (s == nullptr) {
      asdf_logger.warn() << "Could not install a log stream for libasdf; its messages "
                         << "will go to stderr unformatted";
      return stderr;
    }

    return s;
  }();

  return stream;
}


/// The libasdf log level matching SourceXtractor++'s configured one
asdf_log_level_t getAsdfLogLevel() {
  // Elements has no getter for its level, but setLevel() is global, so ask log4cpp
  // what our own logger resolves to.  Lower log4cpp priorities are more severe.
  auto priority = log4cpp::Category::getInstance("libasdf").getChainedPriority();

  // libasdf's TRACE is deliberately never requested: it is very fine-grained and
  // libasdf does real work to format those messages
  if (priority >= log4cpp::Priority::DEBUG) {
    return ASDF_LOG_DEBUG;
  }
  if (priority >= log4cpp::Priority::INFO) {
    return ASDF_LOG_INFO;
  }
  if (priority >= log4cpp::Priority::WARN) {
    return ASDF_LOG_WARN;
  }
  if (priority >= log4cpp::Priority::ERROR) {
    return ASDF_LOG_ERROR;
  }

  return ASDF_LOG_FATAL;
}


/**
 * A libasdf logging configuration that forwards messages into Elements::Logging
 *
 * Colour is asked to be off; libasdf would not colourize this stream anyway, since
 * it is not a terminal, but the level parse in emitAsdfLogLine() depends on it.
 * As previously mentioned, future structured logging support in libasdf will
 * obviate the need for this getAsdfLogStream workaround.
 */
asdf_log_cfg_t getAsdfLogConfig() {
  asdf_log_cfg_t cfg{};

  cfg.stream = getAsdfLogStream();
  cfg.no_color = true;
  // The logger name already says libasdf; keep the source location, the only clue
  // to where in libasdf a message came from
  cfg.fields = ASDF_LOG_FIELD_LEVEL | ASDF_LOG_FIELD_FILE | ASDF_LOG_FIELD_LINE |
               ASDF_LOG_FIELD_MSG;
  cfg.level = getAsdfLogLevel();

  return cfg;
}

}  // anonymous namespace


AsdfFile::AsdfFile(const boost::filesystem::path& path, bool writeable)
    : m_path(path), m_asdf_ptr(nullptr, asdf_close) {

  if (writeable) {
    throw Elements::Exception() << "ASDF files are not currently supported for writing";
  }

  open();
}


AsdfFile::~AsdfFile() {}


void AsdfFile::open() {
  asdf_config_t config{};
  config.log = getAsdfLogConfig();
  asdf_file_t* ptr = asdf_open_file_ex(m_path.native().c_str(), "r", &config);

  // TODO: Actually kinda stinks there's not enough options for retrieving file open errors
  // yet in libasdf; this needs improvement on the libasdf side.
  if (ptr == nullptr) {
      throw Elements::Exception()
          << "Can't open ASDF file: ";
  }

  m_asdf_ptr.reset(ptr);

  // Check if the file was created but has an error condition set
  const char *error_message = asdf_error(ptr);

  if (error_message != nullptr) {
    throw Elements::Exception()
        << "Can't open ASDF file: " << m_path.native() << " reason: " << error_message;
  }
}

// TODO: Support negative indexing as well
// TODO: Probably change this to use asdf_value_find_iter instead, and
// enumerate all ndarray in breadth-first order rather than just top-level.
std::unique_ptr<AsdfFile::Ndarray> AsdfFile::getNdarray(int index) {
  AsdfValuePtr root = getValue("/");

  if (!asdf_value_is_mapping(root.get())) {
    throw AsdfValueNotFoundException() << "ASDF tree root is not a mapping in file: "
      << m_path.native();
  }

  asdf_mapping_t* mapping = nullptr;

  if (ASDF_VALUE_OK != asdf_value_as_mapping(root.get(), &mapping)) {
    throw AsdfValueNotFoundException() << "Could not read the ASDF tree root as a mapping "
      "in file: " << m_path.native();
  }

  asdf_mapping_iter_t* iter = asdf_mapping_iter_init(mapping);
  int count = 0;

  while (asdf_mapping_iter_next(&iter)) {
    if (asdf_value_is_ndarray(iter->value)) {
      if (count == index) {
        // The iterator's value is only valid until the next iteration step, so
        // take an owned copy before breaking out of the loop.  Breaking early
        // also means destroying the iterator by hand; it frees itself only when
        // iteration runs to exhaustion.
        AsdfValuePtr value_copy = AsdfValuePtr(asdf_value_copy(iter->value));
        asdf_mapping_iter_destroy(iter);
        return std::unique_ptr<Ndarray>(new Ndarray(*this, value_copy.get()));
      }
      count++;
    }
  }

  throw AsdfValueNotFoundException() << "No ndarray at index " << index << " in file: "
    << m_path.native();
}


std::unique_ptr<AsdfFile::Ndarray> AsdfFile::getNdarray(const std::string &path) {
  AsdfValuePtr value = getValue(path);
  return std::unique_ptr<Ndarray>(new Ndarray(*this, value.get()));
}


#ifdef WITH_ASDF_GWCS
/**
 * Build an evaluation context from a value known to hold a GWCS
 *
 * Takes ownership of the value.
 */
AsdfFile::GwcsEvalPtr AsdfFile::makeGwcsEval(asdf_value_t* value) {
  AsdfValuePtr value_ptr(value);
  asdf_gwcs_t* gwcs = nullptr;
  asdf_value_err_t err = asdf_value_as_gwcs(value_ptr.get(), &gwcs);

  if (ASDF_VALUE_OK != err || !gwcs) {
    throw AsdfValueTypeMismatchException() << "Value at " << asdf_value_path(value_ptr.get())
      << " could not be read as a GWCS in ASDF file: " << m_path.native();
  }

  asdf_gwcs_err_t gwcs_err = ASDF_GWCS_OK;
  // A null backend selects the first one registered
  asdf_gwcs_eval_t* eval = asdf_gwcs_eval_create(m_asdf_ptr.get(), gwcs, nullptr, &gwcs_err);

  // The evaluation context does not refer back to the GWCS, so release it now
  asdf_gwcs_destroy(gwcs);

  if (!eval) {
    logger.warn() << "The GWCS at " << asdf_value_path(value_ptr.get()) << " in "
      << m_path.native() << " cannot be evaluated by the available backend (error "
      << gwcs_err << ")";
  }

  return GwcsEvalPtr(eval, asdf_gwcs_eval_destroy);
}


/**
 * Condition to use for finding a GWCS object in the ASDF tree in getGwcsEval
 *
 * Currently equivalent simply to asdf_value_is_gwcs.
 */
bool gwcsValuePredicate(asdf_value_t* value) {
  return asdf_value_is_gwcs(value);
}


AsdfFile::GwcsEvalPtr AsdfFile::getGwcsEval() {
  AsdfValuePtr root = getValue("/");

  asdf_value_t* value = asdf_value_find(root.get(), gwcsValuePredicate);

  if (!value) {
    logger.warn() << "No GWCS could be found in the ASDF file: " << m_path.native();
    return GwcsEvalPtr(nullptr, asdf_gwcs_eval_destroy);
  }

  return makeGwcsEval(value);
}


AsdfFile::GwcsEvalPtr AsdfFile::getGwcsEval(const std::string& path) {
  asdf_value_t* value = asdf_get_value(m_asdf_ptr.get(), path.c_str());

  if (!value) {
    throw AsdfValueNotFoundException() << "No value at given GWCS path " << path
      << " in ASDF file: " << m_path.native();
  }

  if (!asdf_value_is_gwcs(value)) {
    AsdfValuePtr value_ptr(value);
    throw AsdfValueTypeMismatchException() << "Value at given GWCS path " << path
      << " is not a GWCS in ASDF file: " << m_path.native();
  }

  return makeGwcsEval(value);
}


#endif /* WITH_ASDF_GWCS */


AsdfFile::AsdfValuePtr AsdfFile::getValue(const std::string& path) {
  asdf_value_t* v = asdf_get_value(m_asdf_ptr.get(), path.c_str());
  if (!v) {
    throw AsdfValueNotFoundException() << "No value at path " << path << " in file: "
      << m_path.native();
  }

  return AsdfValuePtr(v);
}


static ImageTile::ImageType convertDatatypeToImageType(const asdf_datatype_t* datatype) {
  ImageTile::ImageType image_type;

  switch (datatype->type) {
  case ASDF_DATATYPE_FLOAT32:
    image_type = ImageTile::FloatImage;
    break;
  case ASDF_DATATYPE_FLOAT64:
    image_type = ImageTile::DoubleImage;
    break;
  case ASDF_DATATYPE_INT32:
    image_type = ImageTile::IntImage;
    break;
  case ASDF_DATATYPE_UINT32:
    image_type = ImageTile::UIntImage;
    break;
  case ASDF_DATATYPE_INT64:
    image_type = ImageTile::LongLongImage;
    break;
  default:
    // TODO: Support more datatypes supported by ASDF
    // Currently SE++ (and ImageTile::ImageType) is constrainted by the basic BITPIX datatypes
    // supported by FITS.  There's no strong need for that other than the fact that it currently
    // only supports FITS.  Nevertheless for now this will cover most common cases.
    throw AsdfFile::AsdfUnsupportedDatatypeException() << "Unsupported ASDF ndarray datatype: "
      << asdf_scalar_datatype_to_string(datatype->type);
  }

  return image_type;
}


static asdf_scalar_datatype_t convertImageTypeToDatatype(ImageTile::ImageType image_type) {
  // This is the same default used for FITS
  asdf_scalar_datatype_t datatype = ASDF_DATATYPE_FLOAT32;

  switch (image_type) {
  default:
  case ImageTile::FloatImage:
    datatype = ASDF_DATATYPE_FLOAT32;
    break;
  case ImageTile::DoubleImage:
    datatype = ASDF_DATATYPE_FLOAT64;
    break;
  case ImageTile::IntImage:
    datatype = ASDF_DATATYPE_INT32;
    break;
  case ImageTile::UIntImage:
    datatype = ASDF_DATATYPE_UINT32;
    break;
  case ImageTile::LongLongImage:
    datatype = ASDF_DATATYPE_INT64;
    break;
  }

  return datatype;
}


AsdfFile::Ndarray::Ndarray(const AsdfFile& file, asdf_value_t* value)
    : Ndarray((asdf_ndarray_t*)nullptr) {
  asdf_ndarray_t* ndarray_ptr = nullptr;
  asdf_value_err_t err = asdf_value_as_ndarray(value, &ndarray_ptr);
  const char* path = asdf_value_path(value);
  m_path = path;
  switch (err) {
    case ASDF_VALUE_OK:
      // Value exists and is an ndarray: OK
      break;
    case ASDF_VALUE_ERR_TYPE_MISMATCH: {
      throw AsdfValueTypeMismatchException() << "Value at " << path << " is not an ndarray: "
        << file.m_path.native();
    }
    default: {
      const char* error_message = asdf_error(file.getAsdfFilePtr());
      throw AsdfValueNotFoundException() << "An error occurred reading the ASDF file "
        << file.m_path.native() << ": " << error_message;
    }
  }

  m_ndarray_ptr = ndarray_ptr;
  m_image_type = convertDatatypeToImageType(&ndarray_ptr->datatype);
}


void AsdfFile::Ndarray::fillImageTile(const std::shared_ptr<ImageTile> image_tile, int layer) {
  uint64_t plane_origin = layer;
  void* data = image_tile->getDataPtr();
  asdf_ndarray_err_t err = asdf_ndarray_read_tile_2d(
    m_ndarray_ptr,
    image_tile->getPosX(),
    image_tile->getPosY(),
    image_tile->getWidth(),
    image_tile->getHeight(),
    &plane_origin,
    convertImageTypeToDatatype(image_tile->getType()),
    &data
  );

  switch (err) {
    case ASDF_NDARRAY_OK:
      break;
    case ASDF_NDARRAY_ERR_OUT_OF_BOUNDS:
      throw Elements::Exception() << "requested image tile (" << image_tile->getPosX()
        << ", " << image_tile->getPosY() << ") -> ("
        << image_tile->getPosX() + image_tile->getWidth() << ", "
        << image_tile->getPosY() + image_tile->getHeight() << ") is out of bounds";
    case ASDF_NDARRAY_ERR_OOM:
      throw Elements::Exception() << "out of memory copying image tile";
    case ASDF_NDARRAY_ERR_INVAL:
    default:
      throw Elements::Exception() << "invalid argument to asdf_ndarray_read_tile_2d";
  }
}


bool AsdfFile::Ndarray::isSupportedImage() const {
  uint64_t ndim = m_ndarray_ptr->ndim;

  if (ndim < 2 || ndim > 3) {
    return false;
  }

  try {
    getImageType();
  } catch (const AsdfFile::AsdfUnsupportedDatatypeException&) {
    return false;
  }

  return true;
}


}  // namespace SourceXtractor
