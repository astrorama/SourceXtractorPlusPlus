import ElementsKernel.Logging as log

class ExampleClass(object):
    """
    @class ExampleClass
    @brief This is an example of the most basic Python class
    """                
    logger = log.getLogger('ExampleClass')   
    def __init__(self, list):
        """
        @brief Constructor
        """                
        self._m_i = 5
        self._m_list = list
        
    def sumListValues(self):
        """
        @brief Method summing the elements of the list
        """                
        sum = 0
        for element in self._m_list:
            sum += element
        return sum
        
            
    @staticmethod
    def product(first, second):
        """
        @brief Procuct
        
        @details This is an example static method
        
        @param first
             The first number
        @param second
             The second number
        
        @returns
            The product of the two numbers
        """               
        return first * second 

     