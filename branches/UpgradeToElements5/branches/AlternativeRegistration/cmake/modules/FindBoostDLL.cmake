if (NOT BoostDDL_FOUND)

    find_path(BoostDLL_INCLUDE_DIRS boost/dll
              HINTS ENV BOOST_DLL_INSTALL_DIR
              PATH_SUFFIXES include)

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(BoostDDL DEFAULT_MSG BoostDLL_INCLUDE_DIRS)
    mark_as_advanced(BoostDDL_FOUND BoostDLL_INCLUDE_DIRS)

endif()