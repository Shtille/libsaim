#ifndef __SAIM_KEY_PAIR_H__
#define __SAIM_KEY_PAIR_H__

#include "saim_data_key.h"
#include "saim_data_info.h"

#pragma pack(push, 1)

typedef struct {
    saim_data_key key;
    data_info_t info;
} key_pair_t;

typedef struct {
    saim_data_key key;
    stored_data_info_t info;
} stored_key_pair_t;

#pragma pack(pop)

void make_key_pair(key_pair_t * dst, const stored_key_pair_t * src, file_offset_t key_offset);
void make_stored_key_pair(stored_key_pair_t * dst, const key_pair_t * src);

int key_pair_compare(const key_pair_t * a, const key_pair_t * b);

#endif