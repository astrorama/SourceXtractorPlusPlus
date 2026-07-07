CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

#---------------------------------------------------------------
# Load macros and functions for Elements-based projects
find_package(ElementsProject)
#---------------------------------------------------------------

# Declare project name and version
elements_project(SourceXtractorPlusPlus 1.2.0 USE Alexandria 2.33.0 DESCRIPTION "SourceXtractor++, the next generation SExtractor")
