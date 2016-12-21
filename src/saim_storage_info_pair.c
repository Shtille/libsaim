#include "saim_storage_info_pair.h"

void saim_storage_info_pair__create(saim_storage_info_pair * pair)
{
	saim_string_create(&pair->name);
	saim_storage_info__create(&pair->info);
}
void saim_storage_info_pair__destroy(saim_storage_info_pair * pair)
{
	saim_string_destroy(&pair->name);
	saim_storage_info__destroy(&pair->info);
}
int saim_storage_info_pair__compare(const saim_storage_info_pair * a, const saim_storage_info_pair * b)
{
	return saim_string_compare(&a->name, &b->name);
}