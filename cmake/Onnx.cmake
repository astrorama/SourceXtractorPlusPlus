find_package(Protobuf)
find_package(Onnx)

if (Protobuf_FOUND AND Onnx_FOUND)
  # .proto file is available wherever onnx has been installed with pip or conda
  execute_process(COMMAND ${PYTHON_EXECUTABLE} -c "import onnx; print(onnx.__file__)" OUTPUT_VARIABLE ONNX_PYTHON_INIT)
  get_filename_component(ONNX_PYTHON_DIR "${ONNX_PYTHON_INIT}" DIRECTORY CACHE)

  find_path(Onnx_PROTO onnx.proto
          HINTS "${ONNX_PYTHON_DIR}" ENV ONNX_INSTALL_DIR)

  # Generate C++ bindings
  protobuf_generate_cpp(ONNX_SRCS ONNX_HDRS "${Onnx_PROTO}/onnx.proto")
  get_filename_component(Onnx_INCLUDE_DIR "${ONNX_HDRS}" DIRECTORY CACHE)

  elements_add_library(Onnx ${ONNX_SRCS}
             LINK_LIBRARIES ${Protobuf_LIBRARIES}
             INCLUDE_DIRS ${Onnx_INCLUDE_DIR}
             PUBLIC_HEADERS ${Onnx_INCLUDE_DIR})

else()
  message(WARNING "Could not find protobuf, which is needed to compile ONNX proto files")
endif()
