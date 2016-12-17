# Makefile for windows
# This makefile has been made for compilation possibility under systems without Visual Studio 2013

TARGET = saim
ROOT_PATH = .
TARGET_PATH = $(ROOT_PATH)\bin
STATIC_LIB = lib$(TARGET).a
SHARED_LIB = lib$(TARGET).so

ifeq ($(IS_STATIC),NO)
TARGET_TYPE = dynamic
TARGET_FILE = $(SHARED_LIB)
else
TARGET_TYPE = static
TARGET_FILE = $(STATIC_LIB)
endif

CC = gcc
AR = ar rcs

CP = @copy /Y
RM = @del /Q

LIB_PATH = $(ROOT_PATH)\src

INCLUDE += -I$(LIB_PATH) -I$(ROOT_PATH)\include -I$(CURL_PATH)\include

DEFINES = -DBUILDING_LIBSAIM
ifneq ($(IS_STATIC),NO)
DEFINES += -DSAIM_STATICLIB
endif

CFLAGS = -g -Wall -O3 -std=c99
CFLAGS += $(INCLUDE)
CFLAGS += $(DEFINES)

LDFLAGS = -s -shared

SRC_DIRS = $(LIB_PATH)
SRC_DIRS += $(LIB_PATH)/rasterizer
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
	@for /r %%R in (*.o) do (if exist %%R del /Q %%R)
	@if exist $(ROOT_PATH)\bin\$(TARGET_FILE) $(RM) $(ROOT_PATH)\bin\$(TARGET_FILE)
    
static: $(OBJECTS)
	@echo making static library
	@$(AR) $(STATIC_LIB) $(OBJECTS)
	@$(CP) $(STATIC_LIB) $(TARGET_PATH)\$(STATIC_LIB)
	@$(RM) $(STATIC_LIB)
	
dynamic: $(OBJECTS)
	@echo making shared library
	@$(CC) $(LDFLAGS) -o $(SHARED_LIB) $^ $(LIBRARIES)
	@$(CP) $(SHARED_LIB) $(TARGET_PATH)\$(SHARED_LIB)
	@$(RM) $(SHARED_LIB)

%.o : %.c
	@echo compiling file $<
	@$(CC) $(CFLAGS) -c $< -o $@