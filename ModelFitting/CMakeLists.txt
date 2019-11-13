# Declare this directory as an Elements module
elements_subdir(ModelFitting)

# Declare Elements module dependencies
elements_depends_on_subdirs(ElementsKernel)
elements_depends_on_subdirs(SEUtils)

# Declare library dependencies
#    The module is using the Boost C++ library
find_package(Boost)
find_package(Levmar)

# Instruction for creating a C++ library
#    ModelFitting is the library name
#    src/lib/*.cpp are the source files
if (NOT LEVMAR_FOUND)
    set(WITHOUT_LEVMAR ON PARENT_SCOPE)
    add_definitions(-DWITHOUT_LEVMAR)
    message(WARNING "
Levmar library not found, compiling without Levmar support!
This will generate a version of sextractor++ without model fitting")
else ()
    set (LEVMAR Levmar)
endif ()

elements_add_library(ModelFitting
                     src/lib/Parameters/*.cpp
                     src/lib/Engine/*.cpp
                     src/lib/Models/*.cpp
                     LINK_LIBRARIES ElementsKernel Boost ${LEVMAR}
                     INCLUDE_DIRS ${LEVMAR_INCLUDE_DIRS}
                     PUBLIC_HEADERS ModelFitting)

# Instruction for Python module installation
#elements_install_python_modules()

# Install the configuration files
elements_install_conf_files()

# Instruction for building C++ tests
elements_add_unit_test(ManualParameter_test                  
                       tests/src/Parameters/ManualParameter_test.cpp
                       LINK_LIBRARIES ModelFitting TYPE Boost )   
                       
elements_add_unit_test(NeutralConverter_test                  
                       tests/src/Parameters/NeutralConverter_test.cpp
                       LINK_LIBRARIES ModelFitting TYPE Boost )   
                       
elements_add_unit_test(SigmoidConverter_test                  
                       tests/src/Parameters/SigmoidConverter_test.cpp
                       LINK_LIBRARIES ModelFitting TYPE Boost )   
                       
elements_add_unit_test(ExpSigmoidConverter_test                  
                       tests/src/Parameters/ExpSigmoidConverter_test.cpp
                       LINK_LIBRARIES ModelFitting TYPE Boost )   
                       
elements_add_unit_test(EngineParameter_test              
                       tests/src/Parameters/EngineParameter_test.cpp
                       LINK_LIBRARIES ModelFitting TYPE Boost )   
                       
elements_add_unit_test(DependentParameter_test              
                       tests/src/Parameters/DependentParameter_test.cpp
                       LINK_LIBRARIES ModelFitting TYPE Boost )

elements_add_unit_test(SersicProfile_test
                       tests/src/Models/SersicProfile_test.cpp
                       LINK_LIBRARIES ModelFitting TYPE Boost )
elements_add_unit_test(AutoSharp_test
                       tests/src/Models/AutoSharp_test.cpp
                       LINK_LIBRARIES ModelFitting TYPE Boost )
elements_add_unit_test(OldSharp_test
                       tests/src/Models/OldSharp_test.cpp
                       LINK_LIBRARIES ModelFitting TYPE Boost )
elements_add_unit_test(CircularSymmetricModelComponent_test
                       tests/src/Models/CircularlySymmetricModelComponent_test.cpp
                       LINK_LIBRARIES ModelFitting TYPE Boost )
elements_add_unit_test(RotatedModelComponent_test
                       tests/src/Models/RotatedModelComponent_test.cpp
                       LINK_LIBRARIES ModelFitting TYPE Boost )
elements_add_unit_test(ScaledModelComponent_test
                       tests/src/Models/ScaledModelComponent_test.cpp
                       LINK_LIBRARIES ModelFitting TYPE Boost )
elements_add_unit_test(TransformModelComponent_test
                       tests/src/Models/TransformModelComponent_test.cpp
                       LINK_LIBRARIES ModelFitting TYPE Boost )


find_package(CCfits)
find_package(OpenCV QUIET)

if (OPENCV_FOUND)
    elements_add_unit_test(OpenCvMatImageTraits_test
                           tests/src/Image/OpenCvMatImageTraits_test.cpp
                           LINK_LIBRARIES ModelFitting TYPE Boost ${OpenCV_LIBS})
endif ()

if (OPENCV_FOUND AND CCFITS_FOUND)
    elements_add_executable(Example1 src/program/Example1.cpp
                            LINK_LIBRARIES ModelFitting ${CCFITS_LIBRARIES} ${OpenCV_LIBS}
                            INCLUDE_DIRS ${CCFITS_INCLUDE_DIRS})

    elements_add_executable(Example2 src/program/Example2.cpp
                            LINK_LIBRARIES ModelFitting ${CCFITS_LIBRARIES} ${OpenCV_LIBS}
                            INCLUDE_DIRS ${CCFITS_INCLUDE_DIRS})

    elements_add_executable(Example3 src/program/Example3.cpp
                            LINK_LIBRARIES ModelFitting ${CCFITS_LIBRARIES} ${OpenCV_LIBS}
                            INCLUDE_DIRS ${CCFITS_INCLUDE_DIRS})

    elements_add_executable(Example4 src/program/Example4.cpp
                            LINK_LIBRARIES ModelFitting ${CCFITS_LIBRARIES} ${OpenCV_LIBS}
                            INCLUDE_DIRS ${CCFITS_INCLUDE_DIRS})

    elements_add_executable(Example_gal src/program/Example_gal.cpp
                            LINK_LIBRARIES ModelFitting ${CCFITS_LIBRARIES} ${OpenCV_LIBS}
                            INCLUDE_DIRS ${CCFITS_INCLUDE_DIRS})

    elements_add_executable(Example_gal2 src/program/Example_gal2.cpp
                            LINK_LIBRARIES ModelFitting ${CCFITS_LIBRARIES} ${OpenCV_LIBS}
                            INCLUDE_DIRS ${CCFITS_INCLUDE_DIRS})
endif ()
