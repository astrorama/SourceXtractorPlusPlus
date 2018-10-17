CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

#===============================================================================
# Load elements_subdir macro here
# Examples:
#   For declaring a project module:
#         elements_subdir(ElementsExamples)
#===============================================================================
elements_subdir(PythonConfig)

#===============================================================================
# Load elements_depends_on_subdirs macro here
#   For creating a dependency onto an other accessible module
#         elements_depends_on_subdirs(ElementsKernel)
#===============================================================================
elements_depends_on_subdirs(SEUtils)
elements_depends_on_subdirs(SEFramework)
elements_depends_on_subdirs(SEImplementation)
elements_depends_on_subdirs(Configuration) # From Alexandria

#===============================================================================
# Add the find_package macro (a pure CMake command) here to locate the
# libraries.
# Examples:
#          find_package(CppUnit)
#===============================================================================
find_package(PythonInterp REQUIRED)
find_package(PythonLibs REQUIRED)
find_package(Boost REQUIRED COMPONENTS python)

#===============================================================================
# Declare the library dependencies here
# Example:
#         elements_add_library(ElementsExamples src/Lib/*.cpp
#                     LINK_LIBRARIES Boost ElementsKernel
#                     INCLUDE_DIRS Boost ElementsKernel
#                     PUBLIC_HEADERS ElementsExamples)
#===============================================================================
elements_add_library(PythonConfig src/lib/*.cpp
                     LINK_LIBRARIES SEUtils SEFramework SEImplementation Boost PythonLibs PythonInterp Configuration
                     INCLUDE_DIRS ${Boost_INCLUDE_DIR} ${PYTHON_INCLUDE_DIRS}
                     PUBLIC_HEADERS PythonConfig)

#===============================================================================
# Use the following macro for python modules, scripts and aux files:
#  elements_install_python_modules()
#  elements_install_scripts()
#===============================================================================
elements_install_python_modules()
