# Empty if other haven't been set
CURL_PATH :=
CURL_LIB :=

JPEG_PATH := thirdparty/libjpeg
JPEG_LIB := jpeg

PNG_PATH := thirdparty/libpng
PNG_LIB := png

Z_PATH := thirdparty/zlib
Z_LIB := z

ifeq ($(OS),Windows_NT)
    #CCFLAGS += -D WIN32
    MAKE := mingw32-make.exe
    THIRDPARTY_LIBRARIES = zlib png jpeg curl
    SAIM_MAKEFILE = makefile-mingw32.mk
    ZLIB_MAKEFILE = thirdparty/zlib/mingw32.mk
	JPEG_MAKEFILE = thirdparty/libjpeg/mingw32.mk
	PNG_MAKEFILE = thirdparty/libpng/mingw32.mk
	CURL_MAKEFILE = thirdparty/libcurl/mingw32.mk
    CURL_PATH := thirdparty/libcurl
	CURL_LIB := curl
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
	THIRDPARTY_LIBRARIES = zlib png jpeg
	SAIM_MAKEFILE = makefile-unix.mk
	ZLIB_MAKEFILE = thirdparty/zlib/unix.mk
	JPEG_MAKEFILE = thirdparty/libjpeg/unix.mk
	PNG_MAKEFILE = thirdparty/libpng/unix.mk
    INSTALL_PATH := /usr/local/lib
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        #CCFLAGS += -D LINUX
    endif
    ifeq ($(UNAME_S),Darwin)
        #CCFLAGS += -D OSX
        # OSX has its own CURL with command line tools
        CURL_LIB := curl
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
export Z_PATH
export Z_LIB
export INSTALL_PATH

all:
	$(MAKE) -f $(SAIM_MAKEFILE) IS_STATIC=YES
	$(MAKE) -f $(SAIM_MAKEFILE) IS_STATIC=NO

thirdparty: $(THIRDPARTY_LIBRARIES)

zlib:
	$(MAKE) -f $(ZLIB_MAKEFILE) IS_STATIC=YES
	$(MAKE) -f $(ZLIB_MAKEFILE) IS_STATIC=NO

png:
	$(MAKE) -f $(PNG_MAKEFILE) IS_STATIC=YES
	$(MAKE) -f $(PNG_MAKEFILE) IS_STATIC=NO

jpeg:
	$(MAKE) -f $(JPEG_MAKEFILE) IS_STATIC=YES
	$(MAKE) -f $(JPEG_MAKEFILE) IS_STATIC=NO

curl:
	$(MAKE) -f $(CURL_MAKEFILE) IS_STATIC=YES
	$(MAKE) -f $(CURL_MAKEFILE) IS_STATIC=NO