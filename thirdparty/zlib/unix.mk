# Makefile for Unix

TARGET = z
ROOT_PATH = .
TARGET_PATH = $(ROOT_PATH)/bin
STATIC_LIB = lib$(TARGET).a
SHARED_LIB = lib$(TARGET).so

LIB_PATH = $(Z_PATH)/src

ifeq ($(IS_STATIC),NO)
TARGET_TYPE = dynamic
TARGET_FILE = $(SHARED_LIB)
else
TARGET_TYPE = static
TARGET_FILE = $(STATIC_LIB)
endif

CC = gcc
AR = ar rcs

CP = cp
RM = rm -f

INCLUDE += -I$(LIB_PATH) -I$(LIB_PATH)/../include

CFLAGS = -g -Wall -O3 -std=c99
CFLAGS += $(INCLUDE)
CFLAGS += $(DEFINES)

LDFLAGS = -shared -fPIC

SRC_DIRS = $(LIB_PATH)
SRC_FILES = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))

OBJECTS = $(SRC_FILES:.c=.o)

LIBRARIES =

ifeq ($(INSTALL_PATH),)
INSTALL_PATH = $(TARGET_PATH)
endif

all: $(SRC_FILES) $(TARGET)
	@echo All is done!

$(TARGET): create_dir clean $(TARGET_TYPE) install

create_dir:
	@test -d $(INSTALL_PATH) || mkdir $(INSTALL_PATH)

clean:
	@find $(LIB_PATH) -name "*.o" -type f -delete

install:
	@echo installing to $(INSTALL_PATH)
	@$(RM) $(INSTALL_PATH)/$(TARGET_FILE)
	@$(CP) $(TARGET_FILE) $(INSTALL_PATH)/$(TARGET_FILE)
	@$(RM) $(TARGET_FILE)
    
static: $(OBJECTS)
	@echo making static library
	@$(AR) $(STATIC_LIB) $(OBJECTS)
	
dynamic: $(OBJECTS)
	@echo making shared library
	@$(CC) $(LDFLAGS) -o $(SHARED_LIB) $^ $(LIBRARIES)

%.o : %.c
	@echo compiling file $<
	@$(CC) $(CFLAGS) -c $< -o $@