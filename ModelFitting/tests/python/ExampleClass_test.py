import unittest
from ModelFitting.ExamplePythonModule import ExampleClass  # for ExampleClass


class TestCase(unittest.TestCase):
    """ 

    """
    def setUp(self):
        unittest.TestCase.setUp(self)
        self.tol = 1e-6
        self.first = 23.4857
        self.second = 3.4756
        self.my_list = [6, 7, 8, 9]
        self.expected_result = 6 + 7 + 8 + 9
        self.example_object = ExampleClass(self.my_list)      

    def tearDown(self):
        unittest.TestCase.tearDown(self)

    def testProduct(self):
        result = ExampleClass.product(self.first, self.second)
        assert abs(result - (self.first * self.second)) < self.tol

    def testDestruction(self):
        assert abs(self.expected_result - self.example_object.sumListValues()) < self.tol
 
if __name__ == '__main__':
    unittest.main()





