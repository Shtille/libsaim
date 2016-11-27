MAKE := make

ifeq ($(OS),Windows_NT)
    #CCFLAGS += -D WIN32
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
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        #CCFLAGS += -D LINUX
        SAIM_MAKEFILE = makefile-unix.mk
    endif
    ifeq ($(UNAME_S),Darwin)
        #CCFLAGS += -D OSX
        SAIM_MAKEFILE = makefile-unix.mk
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

all:
	$(MAKE) -f $(SAIM_MAKEFILE) IS_STATIC=YES