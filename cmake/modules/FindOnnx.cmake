# - Locate the ONNX library
# Defines:
#
#  Onnx_FOUND
#  Onnx_PROTO

if(NOT Onnx_FOUND)

  find_package(PythonInterp ${PYTHON_EXPLICIT_VERSION} REQUIRED)

  # .proto file is available wherever onnx has been installed with pip or conda
  execute_process(COMMAND ${PYTHON_EXECUTABLE} -c "import onnx; print(onnx.__file__)" OUTPUT_VARIABLE ONNX_PYTHON_INIT)
  get_filename_component(ONNX_PYTHON_DIR "${ONNX_PYTHON_INIT}" DIRECTORY CACHE)

  find_path(Onnx_PROTO onnx.proto
            HINTS "${ONNX_PYTHON_DIR}" ENV ONNX_INSTALL_DIR)

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(Onnx DEFAULT_MSG Onnx_PROTO)

  mark_as_advanced(Onnx_FOUND Onnx_PROTO)

endif()
