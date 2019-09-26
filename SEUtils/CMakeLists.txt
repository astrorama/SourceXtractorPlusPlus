CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

#===============================================================================
# Load elements_subdir macro here
# Examples:
#   For declaring a project module:
#         elements_subdir(ElementsExamples)
#===============================================================================
elements_subdir(SEUtils)

#===============================================================================
# Load elements_depends_on_subdirs macro here
#   For creating a dependency onto an other accessible module
#         elements_depends_on_subdirs(ElementsKernel)
#===============================================================================
elements_depends_on_subdirs(ElementsKernel)

#===============================================================================
# Add the find_package macro (a pure CMake command) here to locate the
# libraries.
# Examples:
#          find_package(CppUnit)
#===============================================================================

find_package(PythonLibs ${PYTHON_EXPLICIT_VERSION} REQUIRED)
find_package(BoostPython ${PYTHON_EXPLICIT_VERSION})
find_package(GMock)

if (${PYTHON_VERSION_MAJOR} VERSION_LESS 3)
    message(DEPRECATION "Python 2 support is deprecated and will be removed (found ${PYTHON_VERSION_STRING})")
endif ()

#===============================================================================
# Declare the library dependencies here
# Example:
#         elements_add_library(ElementsExamples src/Lib/*.cpp
#                     LINK_LIBRARIES Boost ElementsKernel
#                     INCLUDE_DIRS Boost ElementsKernel
#                     PUBLIC_HEADERS ElementsExamples)
#===============================================================================
elements_add_library(SEUtils src/lib/*.cpp
                     LINK_LIBRARIES ElementsKernel BoostPython PythonLibs
                     INCLUDE_DIRS ${BoostPython_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS}
                     PUBLIC_HEADERS SEUtils)

#===============================================================================
# Declare the executables here
# Example:
# elements_add_executable(ElementsProgramExample src/Program/ProgramExample.cpp
#                        LINK_LIBRARIES Boost ElementsExamples
#                        INCLUDE_DIRS Boost ElementsExamples)
#===============================================================================

#===============================================================================
# Declare the Boost tests here
# Example:
# elements_add_unit_test(BoostClassExample tests/src/Boost/ClassExample_test.cpp
#                       EXECUTABLE BoostClassExample_test
#                       INCLUDE_DIRS ElementsExamples
#                       LINK_LIBRARIES ElementsExamples TYPE Boost)
#===============================================================================

elements_add_unit_test(PixelCoordinate_test tests/src/PixelCoordinate_test.cpp 
                     LINK_LIBRARIES SEUtils
                     TYPE Boost)
elements_add_unit_test(NumericalDerivative_test tests/src/NumericalDerivative_test.cpp 
                     LINK_LIBRARIES SEUtils
                     TYPE Boost)

if(GMOCK_FOUND)
elements_add_unit_test(Observable_test tests/src/Observable_test.cpp 
                     LINK_LIBRARIES SEUtils GMock pthread
                     INCLUDE_DIRS GMock
                     TYPE Boost)
endif(GMOCK_FOUND)

#===============================================================================
# Declare the Python programs here
# Examples :
# elements_add_python_program(PythonProgramExample
#                             ElementsExamples.PythonProgramExample)
#===============================================================================

#===============================================================================
# Use the following macro for python modules, scripts and aux files:
#  elements_install_python_modules()
#  elements_install_scripts()
#===============================================================================

#===============================================================================
# Add the elements_install_conf_files macro
# Examples:
#          elements_install_conf_files()
#===============================================================================
