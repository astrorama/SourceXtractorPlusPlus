# - Locate the Levmar library
# Defines:
#
#  Levmar_FOUND
#  Levmar_INCLUDE_DIR
#  Levmar_INCLUDE_DIRS (not cached)
#  Levmar_LIBRARY
#  Levmar_LIBRARIES (not cached)


if(NOT Levmar_FOUND)

    find_path(Levmar_INCLUDE_DIR levmar.h
            HINTS ENV LEVMAR_ROOT_DIR LEVMAR_INSTALL_DIR
            PATH_SUFFIXES include)

    find_library(Levmar_LIBRARY levmar
            HINTS ENV LEVMAR_ROOT_DIR LEVMAR_INSTALL_DIR
            PATH_SUFFIXES lib)

    find_library(M_LIBRARY m)

    set(Levmar_LIBRARIES ${Levmar_LIBRARY})
    set(Levmar_INCLUDE_DIRS ${Levmar_INCLUDE_DIR})

    INCLUDE(FindPackageHandleStandardArgs)
    FIND_PACKAGE_HANDLE_STANDARD_ARGS(Levmar DEFAULT_MSG Levmar_INCLUDE_DIRS Levmar_LIBRARIES)

    mark_as_advanced(Levmar_FOUND Levmar_INCLUDE_DIRS Levmar_LIBRARIES)

    list(REMOVE_DUPLICATES Levmar_LIBRARIES)
    list(REMOVE_DUPLICATES Levmar_INCLUDE_DIRS)

endif()
