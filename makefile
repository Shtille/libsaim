# Makefile for libsaim

# Platform-specific defines
STATIC_LIB_EXT := .a
AR := ar rcs
ifeq ($(OS),Windows_NT)
	#CCFLAGS += -D WIN32
	MAKE := mingw32-make.exe
	LDFLAGS := -s -shared
	CC := gcc
	CXX := g++
	SHARED_LIB_EXT := .so
else
	MAKE := make
	LDFLAGS := -shared -fPIC
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

LIBRARY_DIRS = 
ifneq ($(USE_THIRDPARTY),NO)
LIBRARY_DIRS += thirdparty
endif
BINARY_DIRS = 
DIRS_ORDER = \
	$(LIBRARY_DIRS) saim install_libs \
	$(BINARY_DIRS) install_bins

LIB_PATH = lib

all: $(DIRS_ORDER)

.PHONY: clean
clean:
	@$(foreach directory, $(LIBRARY_DIRS) $(BINARY_DIRS), $(MAKE) -C $(directory) clean ;)
	@$(MAKE) -f saim.mk clean

.PHONY: install
install: install_bins

.PHONY: uninstall
uninstall:
	@$(foreach directory, $(BINARY_DIRS), $(MAKE) -C $(directory) uninstall ;)

.PHONY: help
help:
	@echo available targets: all clean install uninstall

$(LIBRARY_DIRS):
	@$(MAKE) -C $@ $@

$(BINARY_DIRS):
	@$(MAKE) -C $@ $@

saim:
	@$(MAKE) -f saim.mk saim

create_libs_dir:
	@test -d $(LIB_PATH) || mkdir $(LIB_PATH)

install_libs: create_libs_dir
	@find $(LIB_PATH) -name "*$(STATIC_LIB_EXT)" -type f -delete
	@$(foreach directory, $(LIBRARY_DIRS), find $(directory) -name "*$(STATIC_LIB_EXT)" -type f -exec cp {} $(LIB_PATH) \; ;)
	@test -f libsaim$(STATIC_LIB_EXT) && cp libsaim$(STATIC_LIB_EXT) $(LIB_PATH)

install_bins:
	@$(foreach directory, $(BINARY_DIRS), $(MAKE) -C $(directory) install ;)

.PHONY: $(DIRS_ORDER)
