#include "saim_bitmap_info_pair.h"

void saim_bitmap_info_pair__create(saim_bitmap_info_pair * info_pair)
{
	saim_bitmap_info__create(&info_pair->info);
}
void saim_bitmap_info_pair__destroy(saim_bitmap_info_pair * info_pair)
{
	saim_bitmap_info__destroy(&info_pair->info);
}
int saim_bitmap_info_pair__compare(const saim_bitmap_info_pair* p1, const saim_bitmap_info_pair* p2)
{
	// Use key compare function
	return data_key_compare(&p1->key, &p2->key);
}