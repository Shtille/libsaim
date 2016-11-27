#ifndef __SAIM_STRING_H__
#define __SAIM_STRING_H__

typedef struct {
	unsigned int length;
	char * data;
} saim_string;

//! Function to calculate hash from string buffer
unsigned int saim_hash(const char* buffer);
unsigned int saim_string_hash(const saim_string * str);

void saim_string_create(saim_string * str);
void saim_string_destroy(saim_string * str);

void saim_string_set_text(saim_string * str, const char* text);
void saim_string_set(saim_string * str, const saim_string * other);
void saim_string_resize(saim_string * str, unsigned int new_length);
void saim_string_append(saim_string * str, const char* buffer, unsigned int length);

int saim_string_compare(const saim_string * str1, const saim_string * str2);
void saim_string_copy(saim_string * dest, const saim_string * src);

#endif