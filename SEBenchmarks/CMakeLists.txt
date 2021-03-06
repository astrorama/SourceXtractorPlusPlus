CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

#===============================================================================
# Load elements_subdir macro here
# Examples:
#   For declaring a project module:
#         elements_subdir(ElementsExamples)
#===============================================================================
elements_subdir(SEBenchmarks)

#===============================================================================
# Load elements_depends_on_subdirs macro here
#   For creating a dependency onto an other accessible module
#         elements_depends_on_subdirs(ElementsKernel)
#===============================================================================
elements_depends_on_subdirs(AlexandriaKernel)
elements_depends_on_subdirs(ModelFitting)
elements_depends_on_subdirs(SEFramework)
elements_depends_on_subdirs(SEImplementation)

#===============================================================================
# Add the find_package macro (a pure CMake command) here to locate the
# libraries.
# Examples:
#          find_package(CppUnit)
#===============================================================================
find_package(Boost REQUIRED timer)
find_package(OpenCV QUIET)

if (OPENCV_FOUND)
    add_definitions(-DWITH_OPENCV)
endif ()

#===============================================================================
# Declare the library dependencies here
# Example:
#         elements_add_library(ElementsExamples src/Lib/*.cpp
#                     LINK_LIBRARIES Boost ElementsKernel
#                     INCLUDE_DIRS Boost ElementsKernel
#                     PUBLIC_HEADERS ElementsExamples)
#===============================================================================


#===============================================================================
# Declare the executables here
# Example:
# elements_add_executable(ElementsProgramExample src/Program/ProgramExample.cpp
#                        LINK_LIBRARIES Boost ElementsExamples
#                        INCLUDE_DIRS Boost ElementsExamples)
#===============================================================================
elements_add_executable(BenchConvolution src/program/BenchConvolution.cpp
        LINK_LIBRARIES ModelFitting SEFramework ${Boost_LIBRARIES} ${OpenCV_LIBS})
elements_add_executable(BenchBackgroundConvolution src/program/BenchBackgroundConvolution.cpp
        LINK_LIBRARIES SEFramework SEImplementation ${Boost_LIBRARIES})
elements_add_executable(BenchRendering src/program/BenchRendering.cpp
        LINK_LIBRARIES SEFramework SEImplementation ${Boost_LIBRARIES})
elements_add_executable(BenchBackgroundModel src/program/BenchBackgroundModel.cpp
        LINK_LIBRARIES SEFramework SEImplementation ${Boost_LIBRARIES})

#===============================================================================
# Declare the Boost tests here
# Example:
# elements_add_unit_test(BoostClassExample tests/src/Boost/ClassExample_test.cpp
#                       EXECUTABLE BoostClassExample_test
#                       INCLUDE_DIRS ElementsExamples
#                       LINK_LIBRARIES ElementsExamples TYPE Boost)
#===============================================================================

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
