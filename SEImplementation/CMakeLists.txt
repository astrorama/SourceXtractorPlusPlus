CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

#===============================================================================
# Load elements_subdir macro here
# Examples:
#   For declaring a project module:
#         elements_subdir(ElementsExamples)
#===============================================================================
elements_subdir(SEImplementation)

#===============================================================================
# Load elements_depends_on_subdirs macro here
#   For creating a dependency onto an other accessible module
#         elements_depends_on_subdirs(ElementsKernel)
#===============================================================================
elements_depends_on_subdirs(ElementsKernel)
elements_depends_on_subdirs(Configuration) # From Alexandria
elements_depends_on_subdirs(Table) # From Alexandria
elements_depends_on_subdirs(NdArray) # From Alexandria
elements_depends_on_subdirs(SEUtils)
elements_depends_on_subdirs(SEFramework)
elements_depends_on_subdirs(ModelFitting)

#===============================================================================
# Add the find_package macro (a pure CMake command) here to locate the
# libraries.
# Examples:
#          find_package(CppUnit)
#===============================================================================

find_package(yaml-cpp)
find_package(WCSLIB REQUIRED)

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

include_directories(../PythonConfig/)


elements_add_library(SEImplementation
            src/lib/Background/*.cpp
            src/lib/Partition/*.cpp
            src/lib/Segmentation/*.cpp
            src/lib/Grouping/*.cpp
            src/lib/Configuration/*.cpp
            src/lib/PythonConfig/*.cpp
            src/lib/Output/*.cpp
           	src/lib/Measurement/*.cpp
           	src/lib/CoordinateSystem/*.cpp
           	src/lib/CheckImages/*.cpp
            src/lib/Plugin/*/*.cpp
            LINK_LIBRARIES ElementsKernel Configuration SEUtils SEFramework ModelFitting Table WCSLIB yaml-cpp Boost PythonLibs PythonInterp Configuration
            INCLUDE_DIRS ${Boost_INCLUDE_DIR} ${PYTHON_INCLUDE_DIRS}
            PUBLIC_HEADERS SEImplementation)
            
elements_add_executable(tt src/program/tt.cpp
                     LINK_LIBRARIES SEImplementation)

            

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
elements_add_unit_test(PixelCentroid_test tests/src/Plugin/PixelCentroid/PixelCentroid_test.cpp 
                     LINK_LIBRARIES SEImplementation
                     TYPE Boost)
elements_add_unit_test(DetectionFramePixelValues_test tests/src/Plugin/DetectionFramePixelValues/DetectionFramePixelValues_test.cpp 
                     LINK_LIBRARIES SEImplementation
                     TYPE Boost)
elements_add_unit_test(PixelBoundaries_test tests/src/Plugin/PixelBoundaries/PixelBoundaries_test.cpp 
                     LINK_LIBRARIES SEImplementation
                     TYPE Boost)
elements_add_unit_test(DetectionFrameSourceStamp_test tests/src/Plugin/DetectionFrameSourceStamp/DetectionFrameSourceStamp_test.cpp 
                     LINK_LIBRARIES SEImplementation
                     TYPE Boost)
elements_add_unit_test(Lutz_test tests/src/Segmentation/LutzSegmentation_test.cpp 
                     LINK_LIBRARIES SEImplementation
                     TYPE Boost)
elements_add_unit_test(MinAreaPartitionStep_test tests/src/Partition/MinAreaPartitionStep_test.cpp 
                     LINK_LIBRARIES SEImplementation
                     TYPE Boost)
elements_add_unit_test(AttractorsPartitionStep_test tests/src/Partition/AttractorsPartitionStep_test.cpp 
                     LINK_LIBRARIES SEImplementation
                     TYPE Boost)
elements_add_unit_test(MultiThresholdPartitionStep_test tests/src/Partition/MultiThresholdPartitionStep_test.cpp 
                     LINK_LIBRARIES SEImplementation
                     TYPE Boost)
elements_add_unit_test(OverlappingBoundariesCriteria_test tests/src/Grouping/OverlappingBoundariesCriteria_test.cpp 
                     LINK_LIBRARIES SEImplementation
                     TYPE Boost)
elements_add_unit_test(ExternalFlag_test tests/src/Plugin/ExternalFlag/ExternalFlag_test.cpp 
                     LINK_LIBRARIES SEImplementation
                     TYPE Boost)
elements_add_unit_test(AperturePhotometry_test tests/src/Plugin/AperturePhotometry/AperturePhotometry_test.cpp 
                     LINK_LIBRARIES SEImplementation
                     TYPE Boost)
elements_add_unit_test(IsophotalFlux_test tests/src/Plugin/IsophotalFlux/IsophotalFlux_test.cpp 
                     LINK_LIBRARIES SEImplementation
                     TYPE Boost)
elements_add_unit_test(SimpleModelFitting_test tests/src/Plugin/SimpleModelFitting/SimpleModelFitting_test.cpp 
                     LINK_LIBRARIES SEImplementation
                     TYPE Boost)
elements_add_unit_test(PsfTask_test tests/src/Plugin/Psf/PsfTask_test.cpp
                     LINK_LIBRARIES SEImplementation
                     TYPE Boost)
elements_add_unit_test(PointModelFitting_test tests/src/Plugin/PointModelFitting/PointModelFitting_test.cpp
					 LINK_LIBRARIES SEImplementation
					 TYPE Boost)
elements_add_unit_test(ImagePsf_test tests/src/Image/ImagePsf_test.cpp
					 LINK_LIBRARIES SEImplementation
					 TYPE Boost)

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
elements_install_python_modules()

#===============================================================================
# Add the elements_install_conf_files macro
# Examples:
#          elements_install_conf_files()
#===============================================================================
