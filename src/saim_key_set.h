#ifndef __SAIM_KEY_SET_H__
#define __SAIM_KEY_SET_H__

#include "util/saim_set.h"
#include "saim_data_key.h"

typedef struct {
	saim_set * set;
} key_set_t;

void key_set_create(key_set_t * set);
void key_set_destroy(key_set_t * set);

void key_set_clear(key_set_t * set);
void key_set_insert(key_set_t * set, const data_key_t * key);
void key_set_erase(key_set_t * set, saim_set_node * node);
saim_set_node * key_set_search(key_set_t * set, const data_key_t * key);

unsigned int key_set_size(key_set_t * set);

#endif