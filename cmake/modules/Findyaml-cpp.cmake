if(NOT yaml-cpp_FOUND)

  find_path(yaml-cpp_INCLUDE_DIR yaml-cpp/yaml.h)
  find_library(yaml-cpp_LIBRARY yaml-cpp)
  mark_as_advanced(yaml-cpp_INCLUDE_DIR yaml-cpp_LIBRARY)

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(yaml-cpp DEFAULT_MSG
    yaml-cpp_LIBRARY yaml-cpp_INCLUDE_DIR)

  set(yaml-cpp_INCLUDE_DIRS ${yaml-cpp_INCLUDE_DIR})
  set(yaml-cpp_LIBRARIES ${yaml-cpp_LIBRARY})

endif(NOT yaml-cpp_FOUND)
