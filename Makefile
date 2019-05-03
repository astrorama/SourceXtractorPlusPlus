################################################################################
#
# Generic Makefile to simplify the use of CMake projects
# ------------------------------------------------------
#
# This simple Makefile is meant to provide a simplified entry point for the
# configuration and build of CMake-based projects that use a default toolchain
# (as it is the case for Elements-based projects).
#
# Only a few targets are actually provided: all the main targets are directly
# delegated to the CMake Makefile.
#
# Main targets:
#
#     all
#         (default) build everything
#
#     test
#         run the declared tests
#
#     install
#         populate the InstallArea with the products of the build
#
#     clean
#         remove build products from the build directory
#
#     purge [*]_
#         deep clean of the build, including InstallArea
#         (requires re-configuration)
#
#     help
#         print the list of available targets
#
#     configure [*]_
#         alias to CMake 'rebuild_cache' target
#
#     tests [*]_
#         backward-compatibility target for the CMT generic Makefile. Tt
#         ensures that the "all" target has been called before.
#
# :Author: Marco Clemencic
# :Author: Hubert Degaudenzi
#
# .. [*] Targets defined by this Makefile.
#
################################################################################

ELEMENTS_MAKE_LIB := Elements.mk

ifneq ($(wildcard $(CURDIR)/make/$(ELEMENTS_MAKE_LIB)),)
  ELEMENTS_MAKE_LIB_FILE := $(CURDIR)/make/$(ELEMENTS_MAKE_LIB)
else
  ifneq ($(CMAKE_PREFIX_PATH),)
    PREFIX_LIST := $(subst :, ,$(CMAKE_PREFIX_PATH))
    ELEMENTS_MAKE_LIB_LIST := $(foreach dir,$(PREFIX_LIST),$(wildcard $(dir)/share/Elements/make/$(ELEMENTS_MAKE_LIB) $(dir)/../make/$(ELEMENTS_MAKE_LIB)))
  endif
  ELEMENTS_MAKE_LIB_LIST += /usr/share/Elements/make/$(ELEMENTS_MAKE_LIB)
  ELEMENTS_MAKE_LIB_FILE := $(firstword $(ELEMENTS_MAKE_LIB_LIST))
endif

include $(ELEMENTS_MAKE_LIB_FILE)

