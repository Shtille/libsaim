#ifndef __SAIM_FILE_H__
#define __SAIM_FILE_H__

#include "saim_defines.h"
#include "util/saim_string.h"

#include <stdbool.h>
#include <stdio.h>

typedef struct {
	FILE * file;
	char * filename;
	bool operation_successful;
	bool was_open_for_write;
} saim_file;

saim_file * saim_file__alloc(const char* filename);
void saim_file__free(saim_file * file);

//! Create doesn't allocate any data for file struct
void saim_file__create(saim_file * file, const char* filename);
void saim_file__destroy(saim_file * file);

bool saim_file__open_for_write(saim_file * file);
bool saim_file__open_for_read(saim_file * file);
void saim_file__rewind(saim_file * file);
void saim_file__close(saim_file * file);

// Offset operations
void saim_file__offset_from_beginning(saim_file * file, long offset);
void saim_file__offset_from_current(saim_file * file, long offset);
void saim_file__offset_from_end(saim_file * file, long offset);
file_offset_t saim_file__tell(saim_file * file);

// Write operations
void saim_file__write_byte(saim_file * file, unsigned char x);
void saim_file__write_int(saim_file * file, int x);
void saim_file__write_uint(saim_file * file, unsigned int x);
void saim_file__write_array(saim_file * file, const void *data, unsigned int size);
void saim_file__write_string(saim_file * file, const saim_string* str);

// Read operations
void saim_file__read_byte(saim_file * file, unsigned char *x);
void saim_file__read_int(saim_file * file, int *x);
void saim_file__read_uint(saim_file * file, unsigned int *x);
void saim_file__read_array(saim_file * file, void *data, unsigned int size);
void saim_file__read_string(saim_file * file, saim_string *str);

#endif