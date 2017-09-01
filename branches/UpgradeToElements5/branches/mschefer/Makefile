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

# settings
CMAKE := cmake
CTEST := ctest
NINJA := $(shell which ninja-build 2> /dev/null)
ifeq ($(NINJA),)
  NINJA := $(shell which ninja 2> /dev/null)
endif


# Looking for the ToolChain

TOOLCHAIN_NAME := ElementsToolChain.cmake

ifneq ($(wildcard $(CURDIR)/cmake/$(TOOLCHAIN_NAME)),)
  TOOLCHAIN_FILE := $(CURDIR)/cmake/$(TOOLCHAIN_NAME)
else
  ifneq ($(CMAKE_PREFIX_PATH),)
    PREFIX_LIST := $(subst :, ,$(CMAKE_PREFIX_PATH))
    TOOLCHAIN_LIST := $(foreach dir,$(PREFIX_LIST),$(wildcard $(dir)/$(TOOLCHAIN_NAME)))
    TOOLCHAIN_FILE := $(firstword $(TOOLCHAIN_LIST))
  endif
endif

ifneq ($(TOOLCHAIN_FILE),)
  # A toolchain has been found. Lets use it.
  override CMAKEFLAGS += -DCMAKE_TOOLCHAIN_FILE=$(TOOLCHAIN_FILE)
endif



BUILD_PREFIX_NAME := build

override CMAKEFLAGS += -DUSE_LOCAL_INSTALLAREA=ON -DBUILD_PREFIX_NAME:STRING=$(BUILD_PREFIX_NAME)

ifndef BINARY_TAG
  ifdef CMAKECONFIG
    BINARY_TAG := ${CMAKECONFIG}
  else 
    ifdef CMTCONFIG
      BINARY_TAG := ${CMTCONFIG}
    endif
  endif
endif

ifdef BINARY_TAG
  BUILD_SUBDIR := $(BUILD_PREFIX_NAME).$(BINARY_TAG)
else
  BUILD_SUBDIR := $(BUILD_PREFIX_NAME)
endif
BUILDDIR := $(CURDIR)/$(BUILD_SUBDIR)

# build tool

ifneq ($(USE_NINJA),)
  # enable Ninja
  override CMAKEFLAGS += -GNinja
  ifneq ($(VERBOSE),)
    NINJAFLAGS := -v $(NINJAFLAGS)
  endif
  BUILD_CONF_FILE := build.ninja
  BUILD_CMD := $(NINJA) -C $(BUILD_SUBDIR) $(NINJAFLAGS)
#  BUILD_CMD := cd $(BUILD_SUBDIR) && $(NINJA) $(NINJAFLAGS)
else
  BUILD_CONF_FILE := Makefile
  BUILD_CMD := $(MAKE) -C $(BUILD_SUBDIR)
endif



# default target
all:

# deep clean
purge:
	$(RM) -r $(BUILDDIR) $(CURDIR)/InstallArea/$(BINARY_TAG)
	find $(CURDIR) -name "*.pyc" -exec $(RM) -v \{} \;

# delegate any target to the build directory (except 'purge')
ifneq ($(MAKECMDGOALS),purge)
%: $(BUILDDIR)/$(BUILD_CONF_FILE) FORCE
	+$(BUILD_CMD) $*
endif

# aliases
.PHONY: configure tests FORCE
ifneq ($(wildcard $(BUILDDIR)/$(BUILD_CONF_FILE)),)
configure: rebuild_cache
else
configure: $(BUILDDIR)/$(BUILD_CONF_FILE)
endif
	@ # do not delegate further


# This wrapping around the test target is used to ensure the generation of
# the XML output from ctest. 
test: $(BUILDDIR)/$(BUILD_CONF_FILE)
	cd $(BUILDDIR) && $(CTEST) -T Test $(ARGS)


# This target ensures that the "all" target is called before
# running the tests (unlike the "test" default target of CMake)
tests: all
	-$(BUILD_CMD) test

# ensure that the target are always passed to the CMake Makefile
FORCE:
	@ # dummy target

# Makefiles are used as implicit targets in make, but we should not consider
# them for delegation.
$(MAKEFILE_LIST):
	@ # do not delegate further


# trigger CMake configuration
$(BUILDDIR)/$(BUILD_CONF_FILE): | $(BUILDDIR)
	cd $(BUILDDIR) && $(CMAKE) $(CMAKEFLAGS) $(CURDIR)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)
