# - Locate the libasdf library and its optional libasdf-gwcs plugin
# Defines:
#
#  ASDF_FOUND
#  ASDF_INCLUDE_DIR
#  ASDF_INCLUDE_DIRS (not cached)
#  ASDF_LIBRARY
#  ASDF_LIBRARIES (not cached)
#
#  ASDF_GWCS_FOUND
#  ASDF_GWCS_INCLUDE_DIR
#  ASDF_GWCS_INCLUDE_DIRS (not cached)
#  ASDF_GWCS_LIBRARY
#  ASDF_GWCS_LIBRARIES (not cached)
#  ASDF_GWCS_HAS_AST (cached) - whether a usable evaluation backend is present
#
# GWCS support lives in a separate plugin library (libasdf-gwcs) since libasdf
# 0.2.0.  It is detected independently of libasdf itself, so that a libasdf
# without the plugin still counts as found and ASDF images can still be read
# for their pixel data.

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

    if(ASDF_INCLUDE_DIR AND ASDF_LIBRARY AND FYAML_LIBRARY)
      # You can add other deps here if libasdf needs them at link time
      set(ASDF_LIBRARIES ${ASDF_LIBRARY} ${FYAML_LIBRARY})
      set(ASDF_INCLUDE_DIRS ${ASDF_INCLUDE_DIR})
    endif()

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(ASDF
        DEFAULT_MSG
        ASDF_INCLUDE_DIRS ASDF_LIBRARIES)

    mark_as_advanced(ASDF_INCLUDE_DIR ASDF_LIBRARY FYAML_LIBRARY)

    if(ASDF_FOUND)
      list(REMOVE_DUPLICATES ASDF_LIBRARIES)
      list(REMOVE_DUPLICATES ASDF_INCLUDE_DIRS)
    endif()
endif()


#===============================================================================
# libasdf-gwcs: the optional GWCS plugin
#===============================================================================
if(ASDF_FOUND AND NOT ASDF_GWCS_FOUND)

    # NOTE: the header is included as <asdf/gwcs/gwcs.h>, so search for it
    # relative to the directory containing asdf/
    find_path(ASDF_GWCS_INCLUDE_DIR asdf/gwcs/gwcs.h
        HINTS ${ASDF_INCLUDE_DIR} ENV ASDF_ROOT_DIR ASDF_INSTALL_DIR
        PATH_SUFFIXES include)

    find_library(ASDF_GWCS_LIBRARY asdf-gwcs
        HINTS ENV ASDF_ROOT_DIR ASDF_INSTALL_DIR
        PATH_SUFFIXES lib)

    if(ASDF_GWCS_INCLUDE_DIR AND ASDF_GWCS_LIBRARY)
      set(ASDF_GWCS_LIBRARIES ${ASDF_GWCS_LIBRARY})
      set(ASDF_GWCS_INCLUDE_DIRS ${ASDF_GWCS_INCLUDE_DIR})
    endif()

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(ASDF_GWCS
        DEFAULT_MSG
        ASDF_GWCS_INCLUDE_DIRS ASDF_GWCS_LIBRARIES)

    mark_as_advanced(ASDF_GWCS_INCLUDE_DIR ASDF_GWCS_LIBRARY)
endif()


#===============================================================================
# Check that libasdf-gwcs has a usable WCS evaluation backend compiled in.
#
# libasdf-gwcs can be built with or without the Starlink AST backend, and
# without an evaluation backend it can only read GWCS metadata, not evaluate
# it which is the important part here.
#
# This is best implemented as a runtime test to check if instantiating the
# AST evaluation backend actually works.
#
# "ast_yaml" is the only backend that currently exists.  If others appear later
# this should become a loop over the known backend names, perhaps including
# options for selecting a preferred backend.
#===============================================================================
if(ASDF_GWCS_FOUND AND NOT DEFINED ASDF_GWCS_HAS_AST)
    if(CMAKE_CROSSCOMPILING)
      # try_run cannot execute the probe when cross-compiling.  Assume no
      # backend rather than guessing; the cache variable can be set explicitly.
      message(WARNING
        "Cross-compiling: cannot probe libasdf-gwcs for a WCS evaluation backend. "
        "Assuming none is available.  Set ASDF_GWCS_HAS_AST=ON explicitly if the "
        "target's libasdf-gwcs was built with one.")
      set(ASDF_GWCS_HAS_AST OFF CACHE BOOL
          "libasdf-gwcs has a usable WCS evaluation backend")
    else()
      include(CheckCSourceRuns)

      set(_asdf_gwcs_save_includes ${CMAKE_REQUIRED_INCLUDES})
      set(_asdf_gwcs_save_libraries ${CMAKE_REQUIRED_LIBRARIES})
      set(CMAKE_REQUIRED_INCLUDES ${ASDF_GWCS_INCLUDE_DIRS} ${ASDF_INCLUDE_DIRS})
      set(CMAKE_REQUIRED_LIBRARIES ${ASDF_GWCS_LIBRARIES} ${ASDF_LIBRARIES})

      check_c_source_runs("
        #include <asdf/gwcs/gwcs.h>
        int main(void) {
          const asdf_gwcs_backend_t *backend = asdf_gwcs_backend_get(\"ast_yaml\");
          /* A backend with a NULL pipeline vtab is registered but unusable */
          return (backend != NULL && backend->pipeline != NULL) ? 0 : 1;
        }
        " ASDF_GWCS_HAS_AST)

      set(CMAKE_REQUIRED_INCLUDES ${_asdf_gwcs_save_includes})
      set(CMAKE_REQUIRED_LIBRARIES ${_asdf_gwcs_save_libraries})
      unset(_asdf_gwcs_save_includes)
      unset(_asdf_gwcs_save_libraries)
    endif()
endif()
