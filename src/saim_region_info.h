#ifndef __SAIM_REGION_INFO_H__
#define __SAIM_REGION_INFO_H__

#include "util/saim_string.h"

#include <time.h>

#pragma pack(push, 1)

//! Region status
enum {
	kRegion_Stored = 0, // OK
	kRegion_Downloading = 1,
	kRegion_Invalid = 2
};

//! Region information struct
typedef struct {
	double upper_latitude;	//!< upper latitude of bounding box
	double left_longitude;	//!< left longitude of bounding box
	double lower_latitude;	//!< lower latitude of bounding box
	double right_longitude;	//!< right longitude of bounding box
	saim_string name;		//!< name of a region
} region_info_t;

//! Stored region information
typedef struct {
	region_info_t info;		//!< region information
	unsigned int status;	//!< region status
	time_t time;			//!< region data time
	saim_string filename;	//!< region storage filename
} stored_region_info_t;

#pragma pack(pop)

void region_info_create(region_info_t * info);
void region_info_destroy(region_info_t * info);
void region_info_copy(region_info_t * dest, const region_info_t * src);

void stored_region_info_create(stored_region_info_t * info);
void stored_region_info_destroy(stored_region_info_t * info);
void stored_region_info_copy(stored_region_info_t * dst, const stored_region_info_t * src);

#endif