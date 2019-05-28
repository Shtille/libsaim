# Makefile for saim

TARGET = saim
STATIC_LIB = lib$(TARGET)$(STATIC_LIB_EXT)
SHARED_LIB = lib$(TARGET)$(SHARED_LIB_EXT)
LIBRARY_PATH = lib

ifeq ($(IS_STATIC),NO)
TARGET_TYPE = dynamic
TARGET_FILE = $(SHARED_LIB)
else
TARGET_TYPE = static
TARGET_FILE = $(STATIC_LIB)
endif

INCLUDE = -Iinclude -Isrc
INCLUDE += -Ithirdparty/zlib/include
INCLUDE += -Ithirdparty/libpng/include
INCLUDE += -Ithirdparty/libjpeg/include
ifeq ($(OS),Windows_NT)
INCLUDE += -Ithirdparty/libcurl/include
endif

DEFINES = -DBUILDING_LIBSAIM
ifneq ($(IS_STATIC),NO)
DEFINES += -DSAIM_STATICLIB
endif
DEFINES += -DCURL_STATICLIB

SRC_DIRS = \
	src \
	src/rasterizer \
	src/util \
	deps
SRC_FILES = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))

# intermediate directory for generated object files
OBJDIR := .o
# intermediate directory for generated dependency files
DEPDIR := .d

# object files, auto generated from source files
OBJECTS := $(patsubst %,$(OBJDIR)/%.o,$(basename $(SRC_FILES)))
# dependency files, auto generated from source files
DEPS := $(patsubst %,$(DEPDIR)/%.d,$(basename $(SRC_FILES)))

# compilers (at least gcc and clang) don't create the subdirectories automatically
$(shell mkdir -p $(dir $(OBJECTS)) >/dev/null)
$(shell mkdir -p $(dir $(DEPS)) >/dev/null)

# C flags
CFLAGS := -std=c99
# C++ flags
CXXFLAGS := -std=c++99
# C/C++ flags
CPPFLAGS := -g -Wall -O3
#CPPFLAGS += -Wextra -pedantic
CPPFLAGS += $(INCLUDE)
CPPFLAGS += $(DEFINES)
# linker flags
LDFLAGS += -L$(LIBRARY_PATH)
LDLIBS = -lcurl -ljpeg -lpng -lz
ifeq ($(OS),Windows_NT)
	LDLIBS += -lWs2_32 -lWldap32
endif
# flags required for dependency generation; passed to compilers
DEPFLAGS = -MT $@ -MD -MP -MF $(DEPDIR)/$*.Td

# compile C source files
COMPILE.c = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) -c -o $@
# compile C++ source files
COMPILE.cc = $(CXX) $(DEPFLAGS) $(CXXFLAGS) $(CPPFLAGS) -c -o $@
# link object files to static/shared library
ifeq ($(IS_STATIC),NO)
	LINK.o = $(CXX) $(LDFLAGS) $(LDLIBS) -o $@
else
	LINK.o = $(AR) $@
endif
# precompile step
PRECOMPILE =
# postcompile step
ifeq ($(OS),Windows_NT)
	POSTCOMPILE = MOVE /Y $(DEPDIR)\$*.Td $(DEPDIR)\$*.d
else
	POSTCOMPILE = mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d
endif

ifeq ($(OS),Windows_NT)
	CLEAN = rmdir /Q /S $(OBJDIR) && rmdir /Q /S $(DEPDIR)
else
	CLEAN = rm -r $(OBJDIR) $(DEPDIR)
endif

all: $(TARGET)

.PHONY: clean
clean:
	@$(CLEAN)

.PHONY: install
install:
	@echo no install tasks configured

.PHONY: uninstall
uninstall:
	@echo no uninstall tasks configured

.PHONY: help
help:
	@echo available targets: all clean

$(TARGET): $(TARGET_FILE)

$(TARGET_FILE): $(OBJECTS)
	@echo linking $@
	@$(LINK.o) $(OBJECTS)

$(OBJDIR)/%.o: %.c
$(OBJDIR)/%.o: %.c $(DEPDIR)/%.d
	@$(PRECOMPILE)
	@echo compiling $<
	@$(COMPILE.c) $<
	@$(POSTCOMPILE)

$(OBJDIR)/%.o: %.cpp
$(OBJDIR)/%.o: %.cpp $(DEPDIR)/%.d
	@$(PRECOMPILE)
	@echo compiling $<
	@$(COMPILE.cc) $<
	@$(POSTCOMPILE)

.PRECIOUS = $(DEPDIR)/%.d
$(DEPDIR)/%.d: ;

-include $(DEPS)