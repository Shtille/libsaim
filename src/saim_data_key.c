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

void data_key_create(data_key_t * key, int x, int y, int z)
{
	key->hash = hash_function(x, y, z);
}
void data_key_destroy(data_key_t * key)
{
	// Nothing to do
}
void data_key_set(data_key_t * key, int x, int y, int z)
{
	key->hash = hash_function(x, y, z);
}
void data_key_set_by_other(data_key_t * key, const data_key_t * other_key)
{
	key->hash = other_key->hash;
}
int data_key_get_x(const data_key_t * key)
{
	return (int)((key->hash & kXMask)           )-1;
}
int data_key_get_y(const data_key_t * key)
{
	return (int)((key->hash & kYMask) >> kYShift)-1;
}
int data_key_get_z(const data_key_t * key)
{
	return (int)((key->hash & kZMask) >> kZShift)-1;
}
int data_key_compare(const data_key_t * key1, const data_key_t * key2)
{
	if (key1->hash > key2->hash) return 1;
	if (key1->hash < key2->hash) return -1;
	return 0;
}