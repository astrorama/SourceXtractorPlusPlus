CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

#---------------------------------------------------------------
# Load macros and functions for Elements-based projects
find_package(ElementsProject)
#---------------------------------------------------------------

# Declare project name and version
elements_project(SourceXtractorPlusPlus 1.1 USE Alexandria 2.31.4 DESCRIPTION "SourceXtractor++, the next generation SExtractor")
