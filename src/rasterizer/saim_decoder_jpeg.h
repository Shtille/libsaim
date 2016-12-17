#ifndef __SAIM_DECODER_JPEG_H__
#define __SAIM_DECODER_JPEG_H__

#include "saim_bitmap.h"

#include "../util/saim_string.h"

#include <stdlib.h>
#include <stdbool.h>

// TODO
bool saim_decoder_jpeg__check_signature(const char* buffer, size_t length);

//
bool saim_decoder_jpeg__save(const char* filename, int quality, bool inverted_row_order,
	int width, int height, int bpp, const saim_bitmap * bitmap);

//
bool saim_decoder_jpeg__load(const char* filename, bool inverted_row_order,
	int * width, int * height, int * bpp, saim_bitmap * bitmap);

// Don't forget to call free(*dest_ptr) in the end
bool saim_decoder_jpeg__save_to_buffer(const saim_bitmap * bitmap, int quality, bool inverted_row_order,
	int width, int height, int bpp, unsigned char** dest_ptr, unsigned long* dest_size);

//
bool saim_decoder_jpeg__load_from_buffer(const saim_string * data, bool inverted_row_order,
	int * width, int * height, int * bpp, saim_bitmap * bitmap);

#endif