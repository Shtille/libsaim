#ifndef __SAIM_PROVIDER_INFO_H__
#define __SAIM_PROVIDER_INFO_H__

enum {
	SAIM_BOUNDING_BOX		= 1,
	SAIM_LEVEL_OF_DETAIL	= 2,
	SAIM_BITMAP_SIZE		= 4
};

typedef struct {
	double min_latitude;
	double max_latitude;
	double min_longitude;
	double max_longitude;
	int min_lod;
	int max_lod;
	int bitmap_width;
	int bitmap_height;
	char string[260];
} saim_provider_info;

#endif