# - Locate the ONNX runtime library
# Defines:
#
#  OnnxRuntime_FOUND
#  OnnxRuntime_INCLUDE_DIR
#  OnnxRuntime_INCLUDE_DIRS (not cached)
#  OnnxRuntime_LIBRARY
#  OnnxRuntime_LIBRARIES (not cached)

if(NOT OnnxRuntime_FOUND)

  find_path(OnnxRuntime_INCLUDE_DIR onnxruntime_cxx_api.h
            HINTS ENV ONNXRUNTIME_INSTALL_DIR
            PATH_SUFFIXES include onnxruntime onnxruntime/core/session)

  find_library(OnnxRuntime_LIBRARY onnxruntime
               HINTS ENV ONNXRUNTIME_INSTALL_DIR
               PATH_SUFFIXES lib lib64)

  set(OnnxRuntime_INCLUDE_DIRS ${OnnxRuntime_INCLUDE_DIR})
  set(OnnxRuntime_LIBRARIES ${OnnxRuntime_LIBRARY})

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(OnnxRuntime FOUND_VAR OnnxRuntime_FOUND REQUIRED_VARS OnnxRuntime_INCLUDE_DIRS OnnxRuntime_LIBRARIES)

  mark_as_advanced(OnnxRuntime_FOUND OnnxRuntime_INCLUDE_DIRS OnnxRuntime_LIBRARIES)

  list(REMOVE_DUPLICATES OnnxRuntime_INCLUDE_DIRS)
  list(REMOVE_DUPLICATES OnnxRuntime_LIBRARIES)

endif()
