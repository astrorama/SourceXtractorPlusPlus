/**
 * @file ExampleProgram.cpp
 * @date January 6th, 2015
 * @author Pierre Dubath
 */


#include "ElementsKernel/ProgramHeaders.h"  // for including all Program/related headers

#include "ModelFitting/ExampleClass.h"

namespace Euclid {

namespace po = boost::program_options;
namespace fs = boost::filesystem;

using namespace std;

/**
 * @class ExampleProgram
 * @brief
 * 		Simple example of an Elements program
 * @details
 * 		All C++ executable must extend the Elements::Program base class
 *
 */
class ExampleProgram: public Elements::Program {

public:

  /**
   * @brief
   *    Allows to define the (command line and configuration file) options specific to
   *    this program
   * @details
   *    See the ElementsProgram documentation for more details.
   * @return
   *    A BOOST program options_description
   */
  po::options_description defineSpecificProgramOptions() override {
    po::options_description config_options { "Example program options" };

    // Add the specific program options
    config_options.add_options()
        ("string-option",
        po::value<string>()->default_value(string { }),
        "An example string option")
        ("string-option-no-default",
        po::value<string>(), "A string option without default value")
        ("long-long-option", po::value<int64_t>()->default_value(int64_t { }),
        "An example long long option")
        ("double-option",
        po::value<double>()->default_value(double { }),
        "An example double option")
        ("int-vector-option",
        po::value<vector<int>>()->multitoken()->default_value(vector<int> { }, "Empty"),
        "An example vector option");

    return config_options;
  }

  /**
   * @brief
   *    The "main" method.
   * @details
   *    This method is the entry point to the program. In this sense, it is similar to a main
   *    (and it is why it is called mainMethod()). The code below contains the calls to the
   *    different classes created for the first developer's workshop
   *
   *    See the ElementsProgram documentation for more details.
   *
   */
  Elements::ExitCode mainMethod(map<string, po::variable_value>& args)
      override {
    Elements::Logging logger = Elements::Logging::getLogger("ExampleProgram");
    logger.info("Entering mainMethod()");
    logger.info("#");
    /*
     * Check availability of mandatory program arguments (or options)
     *
     * Arguments values may come from
     *  1) the default value provided in above defineSpecificProgramOptions()
     *  2) the configuration file
     *  3) the command line
     *
     *  If an none of the three options provide any values for a mandatory
     *  argument, you should check if your option has any values following the
     *  below example. Note that this may happen for all options without default
     *  values.
     */
    if (args["string-option-no-default"].empty()) {
      logger.info() << "No value are available for string-option-no-default";
      /*
       * An exception may be thrown her if the above option is mandatory and there
       * is no way to continue without value
       */
    }
    /*
     * Get and log one of the program arguments (or options)
     *
     * The string-option has a default empty string value, so that it can always be
     * printed event as an empty string
     */
    std::string string_example { args["string-option"].as<string>() };
    logger.info() << "String option value: " << string_example;

    // Some initialization
    double input_variable = 3.4756;
    int64_t source_id = 12345;
    double ra = 45.637;

    // Factory method example
    ExampleClass example_class_object = ExampleClass::factoryMethod(
        source_id, ra);

    /*
     * All fundamental type variables can be copied forth and back without significant
     * cost in (almost) all cases
     */
    double method_result = example_class_object.fundamentalTypeMethod(
        input_variable);
    logger.info() << "Some result: " << method_result;

    double first = 1.0;
    double second = 0.0;
    double division_result{};
    try {
       logger.info("#");
       logger.info("#   Calling a method throwing an exception ");
       logger.info("#");
       division_result = example_class_object.divideNumbers(first, second);
       //
     } catch (const Elements::Exception & e) {
       logger.info("#");
       logger.info() << e.what();
       logger.info("#");
       logger.info("#   In this silly example we continue with a fake fix ");
       logger.info("#");
       division_result = example_class_object.divideNumbers(first, 0.000001);
     }
     logger.info() << "Second result is: " << division_result;

    /*
     * Illustration on how best to use smart pointer (regular pointer should not
     * be used anymore). The move() indicate that the ownership of the pointer is given to the
     * method called. The vector_unique_ptr cannot be used in this method anymore after the
     * call.
     */
    std::unique_ptr<std::vector<double>> vector_unique_ptr {
      new std::vector<double> { 1.0, 2.3, 4.5 } };
    example_class_object.passingUniquePointer(std::move(vector_unique_ptr));

    /*
     * Illustration on how best to pass any object. The passingObjectInGeneral() is taking
     * a reference to this object.
     */
    std::vector<double> object_example { std::vector<double> { 1.0, 2.3, 4.5 } };
    example_class_object.passingObjectInGeneral(object_example);

    logger.info("#");
    logger.info("Exiting mainMethod()");
    return Elements::ExitCode::OK;
  }

};

} // namespace Euclid

/**
 * Implementation of a main using a base class macro
 * This must be present in all Elements programs
 */
MAIN_FOR(Euclid::ExampleProgram)



