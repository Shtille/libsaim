#ifndef __SAIM_PROVIDER_H__
#define __SAIM_PROVIDER_H__

#include "saim_provider_info.h"
#include "saim_data_key.h"

#include "util/saim_string.h"

#include "../deps/tinycthread.h"

#include <stdbool.h>

typedef struct {
	mtx_t mutex;
	double min_latitude;
	double max_latitude;
	double min_longitude;
	double max_longitude;
	int min_lod;
	int max_lod;
	int bitmap_width;
	int bitmap_height;
	saim_string url_format;
	int passed_key_x;
	int passed_key_y;
	int passed_key_z;
	int * first_argument;
	int * second_argument;
	int * third_argument;
} saim_provider;

bool saim_provider__create(saim_provider * provider);
void saim_provider__destroy(saim_provider * provider);

bool saim_provider__init(saim_provider * provider, saim_provider_info * provider_info, int flags);

void saim_provider__fill_buffer_for_key(saim_provider * provider, const data_key_t * key, char* buffer);

#endif