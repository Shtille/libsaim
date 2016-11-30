#ifndef __SAIM_KEY_REGION_H__
#define __SAIM_KEY_REGION_H__

#include "saim_data_key.h"

#include <stdbool.h>

typedef struct {
	int min_x;
	int max_x;
	int min_y;
	int max_y;
	int level;
	bool is_break;
} key_region_t;

void make_key_region(key_region_t * region, int min_x, int max_x, int min_y, int max_y, int level, bool is_break);

void low_detailed_region(key_region_t * region, const key_region_t * other);
void high_detailed_region(key_region_t * region, const key_region_t * other);

bool is_key_inside_region(const key_region_t * region, const data_key_t * key);

bool key_region_operator_equal(const key_region_t * lhs, const key_region_t * rhs);
bool key_region_operator_less(const key_region_t * lhs, const key_region_t * rhs);

#endif