#include "saim_data_key.h"

#include <string.h>

// Shifts
static const key_hash_t kYShift = 28ULL;
static const key_hash_t kZShift = 56ULL;
// Masks
static const key_hash_t kXMask = 0x000000000FFFFFFFULL;
static const key_hash_t kYMask = 0x00FFFFFFF0000000ULL;
static const key_hash_t kZMask = 0xFF00000000000000ULL;

static key_hash_t hash_function(int x, int y, int z)
{
	key_hash_t ux = (key_hash_t)(x+1);
	key_hash_t uy = (key_hash_t)(y+1);
	key_hash_t uz = (key_hash_t)(z+1);
	return (uz << kZShift) | (uy << kYShift) | (ux);
}

void saim_data_key__create(saim_data_key * key, int x, int y, int z)
{
	key->hash = hash_function(x, y, z);
}
void saim_data_key__destroy(saim_data_key * key)
{
	// Nothing to do
}
void saim_data_key__set(saim_data_key * key, int x, int y, int z)
{
	key->hash = hash_function(x, y, z);
}
void saim_data_key__set_by_other(saim_data_key * key, const saim_data_key * other_key)
{
	key->hash = other_key->hash;
}
int saim_data_key__get_x(const saim_data_key * key)
{
	return (int)((key->hash & kXMask)           )-1;
}
int saim_data_key__get_y(const saim_data_key * key)
{
	return (int)((key->hash & kYMask) >> kYShift)-1;
}
int saim_data_key__get_z(const saim_data_key * key)
{
	return (int)((key->hash & kZMask) >> kZShift)-1;
}
int saim_data_key__compare(const saim_data_key * key1, const saim_data_key * key2)
{
	if (key1->hash > key2->hash) return 1;
	if (key1->hash < key2->hash) return -1;
	return 0;
}