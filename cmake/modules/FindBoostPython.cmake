# Normally, finding a boost component should rely on
#   find_package(Boost REQUIRED COMPONENTS python)
# But the packaging is now always the same: sometimes it will be python3,
# others python37, python2, python27, or just python
# (i.e. python3 for Fedora < 30, but python37 for Fedora >= 30 and MacOSX via Homebrew)
# We wrap all this in this module
#
# Defines:
#   BoostPython_FOUND
#   BoostPython_INCLUDE_DIRS
#   BoostPython_LIBRARIES

if (NOT BoostPython_FOUND)
    find_package(PythonInterp ${PYTHON_EXPLICIT_VERSION} REQUIRED)

    # Explicit versions
    set (_BOOST_PYTHON_LIST "python${PYTHON_VERSION_MAJOR}${PYTHON_VERSION_MINOR}" "python${PYTHON_VERSION_MAJOR}")

    # Only for python2, without suffix
    if (BoostPython_FIND_VERSION VERSION_LESS 3)
        list (APPEND _BOOST_PYTHON_LIST "python")
    endif ()

    # Pick the most restrictive
    foreach (_BOOST_PYTHON IN LISTS _BOOST_PYTHON_LIST)
        find_package(Boost COMPONENTS "${_BOOST_PYTHON}")
        if (Boost_FOUND)
            set (BoostPython_INCLUDE_DIRS ${Boost_INCLUDE_DIRS})
            set (BoostPython_LIBRARIES ${Boost_LIBRARIES})
            set (BoostPython_FOUND TRUE)
            break ()
        endif ()
    endforeach ()

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(BoostPython DEFAULT_MSG BoostPython_INCLUDE_DIRS)
    mark_as_advanced(BoostPython_FOUND BoostPython_INCLUDE_DIRS BoostPython_LIBRARIES)
endif ()
