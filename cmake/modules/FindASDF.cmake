# - Locate the libasdf library
# Defines:
#
#  ASDF_FOUND
#  ASDF_INCLUDE_DIR
#  ASDF_INCLUDE_DIRS (not cached)
#  ASDF_LIBRARY
#  ASDF_LIBRARIES (not cached)

if(NOT ASDF_FOUND)

    # Look for the header
    find_path(ASDF_INCLUDE_DIR asdf.h
        HINTS ENV ASDF_ROOT_DIR ASDF_INSTALL_DIR
        PATH_SUFFIXES include)

    # Look for the library
    find_library(ASDF_LIBRARY asdf
        HINTS ENV ASDF_ROOT_DIR ASDF_INSTALL_DIR
        PATH_SUFFIXES lib)

    # Look for libfyaml which is a dependency of libasdf
    find_library(FYAML_LIBRARY fyaml
        HINTS ENV ASDF_ROOT_DIR ASDF_INSTALL_DIR
        PATH_SUFFIXES lib)

    # You can add other deps here if libasdf needs them at link time
    set(ASDF_LIBRARIES ${ASDF_LIBRARY} ${FYAML_LIBRARY})
    set(ASDF_INCLUDE_DIRS ${ASDF_INCLUDE_DIR})

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(ASDF
        DEFAULT_MSG
        ASDF_INCLUDE_DIRS ASDF_LIBRARIES)

    mark_as_advanced(ASDF_FOUND ASDF_INCLUDE_DIRS ASDF_LIBRARIES)

    list(REMOVE_DUPLICATES ASDF_LIBRARIES)
    list(REMOVE_DUPLICATES ASDF_INCLUDE_DIRS)

endif()
