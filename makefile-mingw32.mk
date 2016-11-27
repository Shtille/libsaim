# Makefile for windows
# This makefile has been made for compilation possibility under systems without Visual Studio 2013

TARGET = saim
ROOT_PATH = .
TARGET_PATH = $(ROOT_PATH)\bin
STATIC_LIB = lib$(TARGET).a
SHARED_LIB = lib$(TARGET).so

ifeq ($(IS_STATIC),"YES")
TARGET_TYPE = static
TARGET_FILE = $(STATIC_LIB)
else
TARGET_TYPE = dynamic
TARGET_FILE = $(SHARED_LIB)
endif

CC = gcc
AR = ar rcs

CP = @copy /Y
RM = @del /Q

LIB_PATH = $(ROOT_PATH)\src

INCLUDE += -I$(LIB_PATH) -I$(ROOT_PATH)\include

DEFINES = -DBUILDING_LIBSAIM
ifeq ($(IS_STATIC),"YES")
DEFINES += -DSAIM_STATICLIB
endif

CFLAGS = -g -Wall -O3 -std=c99
CFLAGS += $(INCLUDE)
CFLAGS += $(DEFINES)

LDFLAGS = -s -shared

SRC_DIRS = $(LIB_PATH)
SRC_DIRS += $(LIB_PATH)/util
SRC_DIRS += $(ROOT_PATH)/deps
SRC_FILES = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))

OBJECTS = $(SRC_FILES:.c=.o)

LIBRARIES =

all: $(SRC_FILES) $(TARGET)
	@echo All is done!

$(TARGET): create_dir clean $(TARGET_TYPE)

create_dir:
	@if not exist $(TARGET_PATH) mkdir $(TARGET_PATH)

clean:
	@del $(ROOT_PATH)\*.o /s
	@del $(ROOT_PATH)\bin\$(TARGET_FILE) /Q
    
static: $(OBJECTS)
	@echo making static library
	@$(AR) $(STATIC_LIB) $(OBJECTS)
	@copy $(STATIC_LIB) $(TARGET_PATH)\$(STATIC_LIB) /Y
	@del $(STATIC_LIB) /Q
	
dynamic: $(OBJECTS)
	@echo making shared library
	@$(CC) $(LDFLAGS) -o $(SHARED_LIB) $^ $(LIBRARIES)
	@copy $(SHARED_LIB) $(TARGET_PATH)\$(SHARED_LIB) /Y
	@del $(SHARED_LIB) /Q

%.o : %.c
	@echo compiling file $<
	@$(CC) $(CFLAGS) -c $< -o $@