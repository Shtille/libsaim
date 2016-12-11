#include "saim_bitmap_info_pair.h"

int saim_bitmap_info_pair__compare(const saim_bitmap_info_pair* p1, const saim_bitmap_info_pair* p2)
{
	// Use key compare function
	return data_key_compare(&p1->key, &p2->key);
}