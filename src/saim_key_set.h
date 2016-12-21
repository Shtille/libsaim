#ifndef __SAIM_KEY_SET_H__
#define __SAIM_KEY_SET_H__

#include "util/saim_set.h"
#include "saim_data_key.h"

typedef struct {
	saim_set * set;
} saim_key_set;

void saim_key_set__create(saim_key_set * set);
void saim_key_set__destroy(saim_key_set * set);

void saim_key_set__clear(saim_key_set * set);
void saim_key_set__insert(saim_key_set * set, const data_key_t * key);
void saim_key_set__erase(saim_key_set * set, saim_set_node * node);
saim_set_node * saim_key_set__search(saim_key_set * set, const data_key_t * key);

unsigned int saim_key_set__size(saim_key_set * set);

#endif