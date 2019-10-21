CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

#===============================================================================
# Load elements_subdir macro here
# Examples:
#   For declaring a project module:
#         elements_subdir(ElementsExamples)
#===============================================================================
elements_subdir(SEMain)

#===============================================================================
# Load elements_depends_on_subdirs macro here
#   For creating a dependency onto an other accessible module
#         elements_depends_on_subdirs(ElementsKernel)
#===============================================================================
elements_depends_on_subdirs(ElementsKernel)
elements_depends_on_subdirs(Configuration)
elements_depends_on_subdirs(SEUtils)
elements_depends_on_subdirs(SEFramework)
elements_depends_on_subdirs(SEImplementation)



#===============================================================================
# Add the find_package macro (a pure CMake command) here to locate the
# libraries.
# Examples:
#          find_package(CppUnit)
#===============================================================================
find_package(Ncurses)
find_package(Readline)

#===============================================================================
# Declare the library dependencies here
# Example:
#         elements_add_library(ElementsExamples src/Lib/*.cpp
#                     LINK_LIBRARIES Boost ElementsKernel
#                     INCLUDE_DIRS Boost ElementsKernel
#                     PUBLIC_HEADERS ElementsExamples)
#===============================================================================
file(GLOB LIB_SRC src/lib/*.cpp)

if (NOT NCURSES_FOUND OR NOT READLINE_FOUND)
    file(GLOB PROGRESSBAR_SRC src/lib/ProgressNCurses.cpp)
    list(REMOVE_ITEM LIB_SRC ${PROGRESSBAR_SRC})
    add_definitions("-DWITHOUT_NCURSES")
else ()
    set (NCURSES Ncurses Readline)
    if (CMAKE_SYSTEM_NAME MATCHES Linux)
        list(APPEND NCURSES rt)
    endif ()
endif ()

elements_add_library(SEMain ${LIB_SRC}
                     LINK_LIBRARIES ElementsKernel Configuration SEImplementation ${NCURSES}
                     PUBLIC_HEADERS SEMain)

#===============================================================================
# Declare the executables here
# Example:
# elements_add_executable(ElementsProgramExample src/Program/ProgramExample.cpp
#                        LINK_LIBRARIES Boost ElementsExamples
#                        INCLUDE_DIRS Boost ElementsExamples)
#===============================================================================
elements_add_executable(sextractor++ src/program/SExtractor.cpp
                     LINK_LIBRARIES SEMain SEUtils SEFramework SEImplementation)

elements_add_executable(TestImage src/program/TestImage.cpp
                     LINK_LIBRARIES SEMain SEUtils SEFramework SEImplementation)

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
elements_install_conf_files()
