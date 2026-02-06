CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

#---------------------------------------------------------------
# Load macros and functions for Elements-based projects
find_package(ElementsProject)
#---------------------------------------------------------------

# Declare project name and version
elements_project(SourceXtractorPlusPlus 1.1.0 USE Alexandria 2.32.0 DESCRIPTION "SourceXtractor++, the next generation SExtractor")


option (TRACY_ENABLE "" ON)
option (TRACY_ON_DEMAND "" ON)
option (TRACY_ONLY_LOCALHOST "" ON)
add_subdirectory (build/tracy)
