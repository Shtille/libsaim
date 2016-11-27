#ifndef __SAIM_DATA_KEY_H__
#define __SAIM_DATA_KEY_H__

//! Define hash type for key struct
typedef unsigned long long key_hash_t;

//! Data key struct
typedef struct {
    key_hash_t hash;
} data_key_t;

void data_key_create(data_key_t * key, int x, int y, int z);
void data_key_destroy(data_key_t * key);

void data_key_set(data_key_t * key, int x, int y, int z);
int data_key_get_x(const data_key_t * key);
int data_key_get_y(const data_key_t * key);
int data_key_get_z(const data_key_t * key);

int data_key_compare(const data_key_t * key1, const data_key_t * key2);

#endif