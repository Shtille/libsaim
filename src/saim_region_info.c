#include "saim_region_info.h"

void region_info_create(region_info_t * info)
{
	saim_string_create(&info->name);
}
void region_info_destroy(region_info_t * info)
{
	saim_string_destroy(&info->name);
}
void region_info_copy(region_info_t * dest, const region_info_t * src)
{
	dest->upper_latitude = src->upper_latitude;
	dest->left_longitude = src->left_longitude;
	dest->lower_latitude = src->lower_latitude;
	dest->right_longitude = src->right_longitude;
	saim_string_copy(&dest->name, &src->name);
}
void stored_region_info_create(stored_region_info_t * info)
{
	region_info_create(&info->info);
	saim_string_create(&info->filename);
}
void stored_region_info_destroy(stored_region_info_t * info)
{
	region_info_destroy(&info->info);
	saim_string_destroy(&info->filename);
}
void stored_region_info_copy(stored_region_info_t * dst, const stored_region_info_t * src)
{
	region_info_copy(&dst->info, &src->info);
	dst->status = src->status;
	dst->time = src->time;
	saim_string_copy(&dst->filename, &src->filename);
}