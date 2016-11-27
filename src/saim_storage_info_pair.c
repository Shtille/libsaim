#include "saim_storage_info_pair.h"

void storage_info_pair_create(storage_info_pair_t * pair)
{
	saim_string_create(&pair->name);
	storage_info_create(&pair->info);
}
void storage_info_pair_destroy(storage_info_pair_t * pair)
{
	saim_string_destroy(&pair->name);
	storage_info_destroy(&pair->info);
}
int storage_info_pair_compare(const storage_info_pair_t * a, const storage_info_pair_t * b)
{
	return saim_string_compare(&a->name, &b->name);
}