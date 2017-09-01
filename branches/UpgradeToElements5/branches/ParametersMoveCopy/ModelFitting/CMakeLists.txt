# Declare this directory as an Elements module
elements_subdir(ModelFitting)

# Declare Elements module dependencies
elements_depends_on_subdirs(ElementsKernel)

# Declare library dependencies
#    The module is using the Boost C++ library
find_package(Boost)

# Instruction for creating a C++ library
#    ModelFitting is the library name
#    src/lib/*.cpp are the source files
elements_add_library(ModelFitting
                     src/lib/Parameters/*.cpp
                     src/lib/Engine/*.cpp
                     LINK_LIBRARIES ElementsKernel Boost levmar
                     PUBLIC_HEADERS ModelFitting)

# Instruction for Python module installation
elements_install_python_modules()

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

elements_add_executable(Temp src/program/Temp.cpp
                        LINK_LIBRARIES ModelFitting)