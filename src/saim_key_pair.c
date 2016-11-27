#include "saim_key_pair.h"

void make_key_pair(key_pair_t * dst, const stored_key_pair_t * src, file_offset_t key_offset)
{
	dst->key = src->key;
	dst->info.key_offset = key_offset;
	dst->info.data_offset = src->info.offset;
	dst->info.size = src->info.size;
	dst->info.counter = src->info.counter;
}
void make_stored_key_pair(stored_key_pair_t * dst, const key_pair_t * src)
{
	dst->key = src->key;
	dst->info.offset = src->info.data_offset;
	dst->info.size = src->info.size;
	dst->info.counter = src->info.counter;
}
int key_pair_compare(const key_pair_t * a, const key_pair_t * b)
{
	// Use key compare function
	return data_key_compare(&a->key, &b->key);
}