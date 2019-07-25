CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

#===============================================================================
# Load elements_subdir macro here
# Examples:
#   For declaring a project module:
#         elements_subdir(ElementsExamples)
#===============================================================================
elements_subdir(SEFramework)

#===============================================================================
# Load elements_depends_on_subdirs macro here
#   For creating a dependency onto an other accessible module
#         elements_depends_on_subdirs(ElementsKernel)
#===============================================================================
elements_depends_on_subdirs(ElementsKernel)
elements_depends_on_subdirs(Configuration) # From Alexandria
elements_depends_on_subdirs(Table)
elements_depends_on_subdirs(SEUtils)
elements_depends_on_subdirs(ModelFitting)

#===============================================================================
# Add the find_package macro (a pure CMake command) here to locate the
# libraries.
# Examples:
#          find_package(CppUnit)
#===============================================================================

find_package(GMock)
find_package(CCfits)
find_package(BoostDLL)
find_package(FFTW COMPONENTS single double long)

#===============================================================================
# Declare the library dependencies here
# Example:
#         elements_add_library(ElementsExamples src/Lib/*.cpp
#                     LINK_LIBRARIES Boost ElementsKernel
#                     INCLUDE_DIRS Boost ElementsKernel
#                     PUBLIC_HEADERS ElementsExamples)
#===============================================================================
elements_add_library(SEFramework
                     src/lib/Aperture/*.cpp
                     src/lib/Pipeline/*.cpp 
                     src/lib/Property/*.cpp
                     src/lib/Source/*.cpp 
                     src/lib/Task/*.cpp
                     src/lib/Output/*.cpp
                     src/lib/Plugin/*.cpp
                     src/lib/Image/*.cpp
                     src/lib/Filesystem/*.cpp
                     src/lib/Psf/*.cpp
                     src/lib/FFT/*.cpp
                     LINK_LIBRARIES ElementsKernel SEUtils Table Configuration ${CCFITS_LIBRARIES} ${FFTW_LIBRARIES}
                     INCLUDE_DIRS SEUtils ${CCFITS_INCLUDE_DIRS} ${BoostDLL_INCLUDE_DIRS} ${FFTW_INCLUDE_DIRS}
                     PUBLIC_HEADERS SEFramework)
                     
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
if(GMOCK_FOUND)
elements_add_unit_test(PropertyHolder_test tests/src/Property/PropertyHolder_test.cpp 
                     LINK_LIBRARIES SEFramework GMock
                     INCLUDE_DIRS GMock
                     TYPE Boost)
elements_add_unit_test(SourceWithOnDemandProperties_test tests/src/Source/SourceWithOnDemandProperties_test.cpp 
                     LINK_LIBRARIES SEFramework GMock
                     INCLUDE_DIRS GMock
                     TYPE Boost)
elements_add_unit_test(SourceInterface_test tests/src/Source/SourceInterface_test.cpp 
                     LINK_LIBRARIES SEFramework GMock
                     INCLUDE_DIRS GMock
                     TYPE Boost)
elements_add_unit_test(Partition_test tests/src/Pipeline/Partition_test.cpp 
                     LINK_LIBRARIES SEFramework GMock
                     INCLUDE_DIRS GMock
                     TYPE Boost)
elements_add_unit_test(SourceGroupWithOnDemandProperties_test tests/src/Source/SourceGroupWithOnDemandProperties_test.cpp 
                     LINK_LIBRARIES SEFramework GMock
                     TYPE Boost)
endif(GMOCK_FOUND)
elements_add_unit_test(TaskProvider_test tests/src/Task/TaskProvider_test.cpp 
                     LINK_LIBRARIES SEFramework
                     TYPE Boost)
elements_add_unit_test(PropertyId_test tests/src/Property/PropertyId_test.cpp 
                     LINK_LIBRARIES SEFramework
                     TYPE Boost)
elements_add_unit_test(SourceGrouping_test tests/src/Pipeline/SourceGrouping_test.cpp 
                     LINK_LIBRARIES SEFramework
                     TYPE Boost)
elements_add_unit_test(Deblending_test tests/src/Pipeline/Deblending_test.cpp 
                     LINK_LIBRARIES SEFramework
                     TYPE Boost)
elements_add_unit_test(ImageFitsReader_test tests/src/Image/FitsReader_test.cpp 
                     LINK_LIBRARIES SEFramework
                     TYPE Boost)
elements_add_unit_test(VectorImage_test tests/src/Image/VectorImage_test.cpp 
                     LINK_LIBRARIES SEFramework
                     TYPE Boost)
elements_add_unit_test(ImageChunk_test tests/src/Image/ImageChunk_test.cpp 
                     LINK_LIBRARIES SEFramework
                     TYPE Boost)
elements_add_unit_test(SimpleSource_test tests/src/Source/SimpleSource_test.cpp
                     LINK_LIBRARIES SEFramework
                     TYPE Boost)
elements_add_unit_test(TemporaryFile_test tests/src/Filesystem/TemporaryFile_test.cpp
                     LINK_LIBRARIES SEFramework
                     TYPE Boost)
elements_add_unit_test(TemporaryFitsSource_test tests/src/Image/TemporaryFitsSource_test.cpp
                     LINK_LIBRARIES SEFramework
                     TYPE Boost)
elements_add_unit_test(VariablePsf_test tests/src/Psf/VariablePsf_test.cpp
                     LINK_LIBRARIES SEFramework
                     TYPE Boost)
elements_add_unit_test(PaddedImage_test tests/src/Image/PaddedImage_test.cpp
                     LINK_LIBRARIES SEFramework
                     TYPE Boost)
elements_add_unit_test(RecenterImage_test tests/src/Image/RecenterImage_test.cpp
                     LINK_LIBRARIES SEFramework
                     TYPE Boost)
elements_add_unit_test(MirrorImage_test tests/src/Image/MirrorImage_test.cpp
                     LINK_LIBRARIES SEFramework
                     TYPE Boost)
elements_add_unit_test(FunctionalImage_test tests/src/Image/FunctionalImage_test.cpp
                     LINK_LIBRARIES SEFramework
                     TYPE Boost)
elements_add_unit_test(BufferedImage_test tests/src/Image/BufferedImage_test.cpp
                     LINK_LIBRARIES SEFramework
                     TYPE Boost)
elements_add_unit_test(InterpolatedImage_test tests/src/Image/InterpolatedImage_test.cpp
                     LINK_LIBRARIES SEFramework
                     TYPE Boost)
elements_add_unit_test(InterpolatedImageSource_test tests/src/Image/InterpolatedImageSource_test.cpp
                     LINK_LIBRARIES SEFramework
                     TYPE Boost)
elements_add_unit_test(FFT_test tests/src/FFT/FFT_test.cpp
                     LINK_LIBRARIES SEFramework
                     TYPE Boost)
elements_add_unit_test(DirectConvolution_test tests/src/Convolution/DirectConvolution_test.cpp
                     LINK_LIBRARIES SEFramework
                     TYPE Boost)
elements_add_unit_test(DFT_test tests/src/Convolution/DFT_test.cpp
                     LINK_LIBRARIES SEFramework
                     TYPE Boost)
elements_add_unit_test(TransformedAperture_test tests/src/Aperture/TransformedAperture_test.cpp
                     LINK_LIBRARIES SEFramework
                     TYPE Boost)
elements_add_unit_test(NeighbourInfo_test tests/src/Aperture/NeighbourInfo_test.cpp
                     LINK_LIBRARIES SEFramework
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

#===============================================================================
# Add the elements_install_conf_files macro
# Examples:
#          elements_install_conf_files()
#===============================================================================
