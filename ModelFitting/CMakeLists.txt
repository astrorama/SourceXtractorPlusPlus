# Declare this directory as an Elements module
elements_subdir(ModelFitting)

# Declare Elements module dependencies
elements_depends_on_subdirs(ElementsKernel)
elements_depends_on_subdirs(SEUtils)

# Declare library dependencies
#    The module is using the Boost C++ library
find_package(Boost)
find_package(Levmar)
find_package(GSL 2.2.1)

# Instruction for creating a C++ library
#    ModelFitting is the library name
#    src/lib/*.cpp are the source files
file (GLOB ENGINE_SRC src/lib/Engine/*.cpp)

if (NOT LEVMAR_FOUND)
    file(GLOB LEVMAR_SRC src/lib/Engine/LevmarEngine.cpp)
    list(REMOVE_ITEM  ENGINE_SRC ${LEVMAR_SRC})
    message("Levmar library not found, compiling without Levmar support!")
else ()
    set (LEVMAR Levmar)
endif ()
if (NOT GSL_FOUND)
    file(GLOB GSL_SRC src/lib/Engine/GSLEngine.cpp)
    list(REMOVE_ITEM ENGINE_SRC ${GSL_SRC})
    message("GSL >= 2.2.1 library not found, compiling without GSL support!")
else ()
    set (GSL GSL)
endif ()

if (NOT LEVMAR_FOUND AND NOT GSL_FOUND)
    set(WITHOUT_MODELFITTING ON PARENT_SCOPE)
    message(WARNING "Levmar and GSL not found. This will generate a version of sourcextractor++ without model fitting")
endif ()

elements_add_library(ModelFitting
                     src/lib/Parameters/*.cpp
                     ${ENGINE_SRC}
                     src/lib/Models/*.cpp
                     LINK_LIBRARIES ElementsKernel Boost ${LEVMAR} ${GSL}
                     INCLUDE_DIRS ${LEVMAR_INCLUDE_DIRS} ${GSL_INCLUDE_DIRS}
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

if (OPENCV_FOUND AND CCFITS_FOUND AND FALSE)
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

    elements_add_executable(Example_SimpleFit src/program/Example_SimpleFit.cpp
                            LINK_LIBRARIES ModelFitting ${CCFITS_LIBRARIES} ${OpenCV_LIBS})
endif ()
