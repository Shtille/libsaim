# Makefile for libsaim
#
# Possible options:
# - USE_THIRDPARTY=NO to provide your own libraries (zlib, jpeg, png, curl)
# - SAIM_MAX_TILE_SERVICE_COUNT=8 to set max service count for saim (dafault value is 4)
# 

# Platform-specific defines
STATIC_LIB_EXT := .a
AR := ar rcs
ifeq ($(OS),Windows_NT)
	#CCFLAGS += -D WIN32
	MAKE := mingw32-make.exe
	LDFLAGS := -s -shared
	LIBRARY_PATH ?= $(shell cd)\\lib
	CC := gcc
	CXX := g++
	SHARED_LIB_EXT := .so
else
	MAKE := make
	LDFLAGS := -shared -fPIC
	LIBRARY_PATH ?= $(shell pwd)/lib
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		#CCFLAGS += -D LINUX
		CC := gcc
		CXX := g++
		SHARED_LIB_EXT := .so
	endif
	ifeq ($(UNAME_S),Darwin)
		#CCFLAGS += -D OSX
		CC := clang
		CXX := clang++
		SHARED_LIB_EXT := .dylib
	endif
endif

# Exports
export STATIC_LIB_EXT
export SHARED_LIB_EXT
export CC
export CXX
export AR
export LDFLAGS
export LIBRARY_PATH

LIBRARY_DIRS = 
ifneq ($(USE_THIRDPARTY),NO)
	LIBRARY_DIRS += thirdparty
endif
BINARY_DIRS = 
ifeq ($(OS),Windows_NT)
	BINARY_DIRS += examples
endif
DIRS_ORDER = \
	create_libs_dir $(LIBRARY_DIRS) saim \
	$(BINARY_DIRS)

ifeq ($(OS),Windows_NT)
	CREATE_LIBS_DIR = if not exist $(LIBRARY_PATH) mkdir $(LIBRARY_PATH)
else
	CREATE_LIBS_DIR = test -d $(LIBRARY_PATH) || mkdir -p $(LIBRARY_PATH)
endif

all: $(DIRS_ORDER)

.PHONY: clean
clean:
	@$(foreach directory, $(LIBRARY_DIRS) $(BINARY_DIRS), $(MAKE) -C $(directory) clean ;)
	@$(MAKE) -f saim.mk clean

.PHONY: help
help:
	@echo available targets: all clean

$(LIBRARY_DIRS):
	@$(MAKE) -C $@ $@

$(BINARY_DIRS):
	@$(MAKE) -C $@ $@

saim:
	@$(MAKE) -f saim.mk saim

create_libs_dir:
	@$(CREATE_LIBS_DIR)

.PHONY: $(DIRS_ORDER)
