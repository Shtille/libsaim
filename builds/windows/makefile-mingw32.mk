# Makefile for windows
# This makefile has been made for compilation possibility under systems without Visual Studio 2013

TARGET = saim
ROOT_PATH = ..\..
TARGET_PATH = $(ROOT_PATH)\bin
STATIC_LIB = lib$(TARGET).a
SHARED_LIB = lib$(TARGET).so

CC = gcc
AR = ar

CP = @copy /Y
RM = @del /Q

LIB_PATH = $(ROOT_PATH)\src

INCLUDE += -I$(LIB_PATH) -I$(ROOT_PATH)\include

DEFINES_STATIC = -DBUILDING_LIBSAIM -DSAIM_STATICLIB
DEFINES_DYNAMIC = -DBUILDING_LIBSAIM

CFLAGS = -g -Wall -O3 -std=c99
CFLAGS += $(INCLUDE)

CFLAGS_STATIC = $(CFLAGS) $(DEFINES_STATIC)
CFLAGS_DYNAMIC = $(CFLAGS) $(DEFINES_DYNAMIC)

LDFLAGS = -s -shared

SRC_DIRS = $(LIB_PATH)
SRC_FILES = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))

OBJECTS_STATIC = $(SRC_FILES:.c=.o.s)
OBJECTS_DYNAMIC = $(SRC_FILES:.c=.o.d)

LIBRARIES = -lstdc++

all: $(SRC_FILES) $(TARGET)
	@echo All is done!

$(TARGET): create_dir clean $(STATIC_LIB) $(SHARED_LIB)

create_dir:
	@if not exist $(TARGET_PATH) mkdir $(TARGET_PATH)

clean:
	@del ..\..\*.o.s /s
	@del ..\..\*.o.d /s
	@del ..\..\bin\* /Q
    
$(STATIC_LIB): $(OBJECTS_STATIC)
	$(AR) rcs $@ $(OBJECTS_STATIC)
	$(CP) $(STATIC_LIB) $(TARGET_PATH)\$(STATIC_LIB)
	$(RM) $(STATIC_LIB)
	
$(SHARED_LIB): $(OBJECTS_DYNAMIC)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBRARIES)
	$(CP) $(SHARED_LIB) $(TARGET_PATH)\$(SHARED_LIB)
	$(RM) $(SHARED_LIB)

%.o.s : %.c
	$(CC) $(CFLAGS_STATIC) -c $< -o $@

%.o.d : %.c
	$(CC) $(CFLAGS_DYNAMIC) -c $< -o $@