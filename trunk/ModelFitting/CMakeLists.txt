# Declare this directory as an Elements module
#_  (Replace ExampleModule with the correct name for another module)
elements_subdir(ModelFitting)

# Declare Elements module dependencies
#    This module is using of the ElementsKernel module
elements_depends_on_subdirs(ElementsKernel)

# Declare library dependencies
#    The module is using the Boost C++ library
find_package(Boost)

# Instruction for creating a C++ library
#    ModelFitting is the library name
#    src/lib/*.cpp are the source files
elements_add_library(ModelFitting src/lib/*.cpp
                     LINK_LIBRARIES ElementsKernel Boost
                     PUBLIC_HEADERS ModelFitting)

# Instruction for creating a C++ executable
#    exampleCppProgram is the executable name
#    src/program/ExampleProgram.cpp is the source file
elements_add_executable(exampleCppProgram src/program/ExampleProgram.cpp
                        LINK_LIBRARIES ModelFitting
                        INCLUDE_DIRS ModelFitting)

# Instruction for Python module installation
elements_install_python_modules()

# Install the configuration files
elements_install_conf_files()

# Instruction for creating a Python executable
#    examplePyProgram is the executable name
#    ModelFitting.ExampleProgram is the source file
elements_add_python_program(examplePyProgram ModelFitting.ExampleProgram)

# Instruction for building C++ tests
#    ExampleClass_test is the name of the test executable
#    tests/src/ExampleClass_test.cpp is the source file
elements_add_unit_test(ExampleClass_test                  
                       tests/src/ExampleClass_test.cpp
                       LINK_LIBRARIES ModelFitting TYPE Boost )   
                       
elements_add_unit_test(ManualParameter_test                  
                       tests/src/ManualParameter_test.cpp
                       LINK_LIBRARIES ModelFitting TYPE Boost )   
                       
elements_add_unit_test(NeutralConverter_test                  
                       tests/src/NeutralConverter_test.cpp
                       LINK_LIBRARIES ModelFitting TYPE Boost )   
                       
elements_add_unit_test(SigmoidConverter_test                  
                       tests/src/SigmoidConverter_test.cpp
                       LINK_LIBRARIES ModelFitting TYPE Boost )   
                       
elements_add_unit_test(ExpSigmoidConverter_test                  
                       tests/src/ExpSigmoidConverter_test.cpp
                       LINK_LIBRARIES ModelFitting TYPE Boost )   
                       
elements_add_unit_test(EngineParameter_test              
                       tests/src/EngineParameter_test.cpp
                       LINK_LIBRARIES ModelFitting TYPE Boost )   
                       
elements_add_unit_test(DependentParameter_test              
                       tests/src/DependentParameter_test.cpp
                       LINK_LIBRARIES ModelFitting TYPE Boost )            
                       
                       
                       
                       
                                          