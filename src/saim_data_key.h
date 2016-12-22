#ifndef __SAIM_DATA_KEY_H__
#define __SAIM_DATA_KEY_H__

//! Define hash type for key struct
typedef unsigned long long key_hash_t;

//! Data key struct
typedef struct {
    key_hash_t hash;
} saim_data_key;

void saim_data_key__create(saim_data_key * key, int x, int y, int z);
void saim_data_key__destroy(saim_data_key * key);

void saim_data_key__set(saim_data_key * key, int x, int y, int z);
void saim_data_key__set_by_other(saim_data_key * key, const saim_data_key * other_key);
int saim_data_key__get_x(const saim_data_key * key);
int saim_data_key__get_y(const saim_data_key * key);
int saim_data_key__get_z(const saim_data_key * key);

int saim_data_key__compare(const saim_data_key * key1, const saim_data_key * key2);

#endif