if (NOT BoostDLL_FOUND)

    find_path(BoostDLL_INCLUDE_DIRS boost/dll
              HINTS ENV BOOST_DLL_INSTALL_DIR
              PATH_SUFFIXES include)

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(BoostDLL DEFAULT_MSG BoostDLL_INCLUDE_DIRS)
    mark_as_advanced(BoostDLL_FOUND BoostDLL_INCLUDE_DIRS)

endif()

