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
} saim_region_info;

//! Stored region information
typedef struct {
	saim_region_info info;		//!< region information
	unsigned int status;	//!< region status
	time_t time;			//!< region data time
	saim_string filename;	//!< region storage filename
} saim_stored_region_info;

#pragma pack(pop)

void saim_region_info__create(saim_region_info * info);
void saim_region_info__destroy(saim_region_info * info);
void saim_region_info__copy(saim_region_info * dest, const saim_region_info * src);

void saim_stored_region_info__create(saim_stored_region_info * info);
void saim_stored_region_info__destroy(saim_stored_region_info * info);
void saim_stored_region_info__copy(saim_stored_region_info * dst, const saim_stored_region_info * src);

#endif