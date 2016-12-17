# Empty if other haven't been set
CURL_PATH :=
CURL_LIB :=

JPEG_PATH :=
JPEG_LIB :=

PNG_PATH :=
PNG_LIB :=

ifeq ($(OS),Windows_NT)
    #CCFLAGS += -D WIN32
    MAKE := mingw32-make.exe
    SAIM_MAKEFILE = makefile-mingw32.mk
    ifeq ($(PROCESSOR_ARCHITEW6432),AMD64)
        #CCFLAGS += -D AMD64
    else
        ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
            #CCFLAGS += -D AMD64
        endif
        ifeq ($(PROCESSOR_ARCHITECTURE),x86)
            #CCFLAGS += -D IA32
        endif
    endif
else
	MAKE := make
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        #CCFLAGS += -D LINUX
        SAIM_MAKEFILE = makefile-unix.mk
        INSTALL_PATH := /usr/local/lib
    endif
    ifeq ($(UNAME_S),Darwin)
        #CCFLAGS += -D OSX
        SAIM_MAKEFILE = makefile-unix.mk
        INSTALL_PATH := /usr/local/lib
    endif
    UNAME_P := $(shell uname -p)
    ifeq ($(UNAME_P),x86_64)
        #CCFLAGS += -D AMD64
    endif
    ifneq ($(filter %86,$(UNAME_P)),)
        #CCFLAGS += -D IA32
    endif
    ifneq ($(filter arm%,$(UNAME_P)),)
        #CCFLAGS += -D ARM
    endif
endif

export CURL_PATH
export CURL_LIB
export JPEG_PATH
export JPEG_LIB
export PNG_PATH
export PNG_LIB
export INSTALL_PATH

all:
	$(MAKE) -f $(SAIM_MAKEFILE) IS_STATIC=YES
	$(MAKE) -f $(SAIM_MAKEFILE) IS_STATIC=NO