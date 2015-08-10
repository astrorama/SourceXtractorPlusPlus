import os                                 # for the path tools
import argparse                           # for program options from configuration
import ElementsKernel.Logging as log      # for Elements logging support
from ModelFitting.ExamplePythonModule import ExampleClass  # for ExampleClass

def defineSpecificProgramOptions():
    """
    @brief Allows to define the (command line and configuration file) options specific to
    this program
    
    @details
        See the ElementsProgram documentation for more details.
    @return
        An  ArgumentParser. 
    """                
    parser = argparse.ArgumentParser()
    parser.add_argument('--string-option', type=str,
                        help='A string example option')
    parser.add_argument('--int-option', type=int,
                        help='An int example option')
    parser.add_argument('--float-option', type=float,
                        help='A float option')
    parser.add_argument('--int-list-option', nargs='+', type=int,
                        help='A integer list option')
    return parser

def mainMethod(args):
    """
    @brief The "main" method.
    @details
        This method is the entry point to the program. In this sense, it is similar to a main
        (and it is why it is called mainMethod()). The code below contains the calls to the
        different classes created for the first developer's workshop

        See the ElementsProgram documentation for more details. 
    """
    logger = log.getLogger('ExampleProgram')
    logger.info('Entering ExampleProgram mainMethod()')
    
    #
    #  Log some of the arguments
    #
    string_from_configuration = args.string_option
    logger.info('Example string : ' + string_from_configuration)
    second_element = args.int_list_option[1]
    logger.info('Second elements from the list : ' + str(second_element) )
    
    #
    #  Calling a static method
    #
    result = ExampleClass.product(3, 4)
    logger.info('Static method result : ' + str(result) )
    
    #
    #  Calling the constructor and a method member
    #
    example_object = ExampleClass(args.int_list_option)
    logger.info('Sum of the list elements : ' + str(example_object.sumListValues()))
    
    logger.info('Exiting ExampleProgram mainMethod()')