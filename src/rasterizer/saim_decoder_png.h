#ifndef __SAIM_DECODER_PNG_H__
#define __SAIM_DECODER_PNG_H__

#include "saim_bitmap.h"

#include "../util/saim_string.h"

#include <stdlib.h>
#include <stdbool.h>

bool saim_decoder_png__check_signature(const unsigned char* buffer, size_t length);

//
bool saim_decoder_png__save(const char* filename, bool inverted_row_order,
	int width, int height, int bpp, const saim_bitmap * bitmap);

//
bool saim_decoder_png__load(const char* filename, bool inverted_row_order,
	int * width, int * height, int * bpp, saim_bitmap * bitmap);

//
bool saim_decoder_png__save_to_buffer(const saim_bitmap * bitmap, bool inverted_row_order,
	int width, int height, int bpp, saim_string * data);

//
bool saim_decoder_png__load_from_buffer(const saim_string * data, bool inverted_row_order,
	int * width, int * height, int * bpp, saim_bitmap * bitmap);

#endif