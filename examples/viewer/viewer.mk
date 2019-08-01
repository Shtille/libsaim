# Makefile for windows
# This makefile has been made for compilation possibility under systems without Visual Studio 2013

SAIM_BIN = ../../lib

SRC_FILES = $(wildcard *.cpp)

INCLUDE = -I../../include

CC = g++
CFLAGS = -g -Wall -O3 -std=c++11
CFLAGS += $(INCLUDE)
LDFLAGS = -s -mwindows
OBJECTS = $(SRC_FILES:.cpp=.o)
TARGET = viewer
TARGET_PATH = ../bin
EXECUTABLE = $(TARGET).exe

LIBRARIES = \
	-lsaim -lcurl -ljpeg -lpng -lz -lWs2_32 -lWldap32 -lgdi32

all: $(SRC_FILES) $(EXECUTABLE)
	echo All is done!
	
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBRARIES) -L$(SAIM_BIN)

%.o : %.cpp
	$(CC) $(CFLAGS) -c $< -o $@