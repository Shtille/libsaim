#ifndef __SAIM_DEFINES_H__
#define __SAIM_DEFINES_H__

//! File offset type
// Note: the file can't be larger than 2 Gb to let offsets work.
typedef int file_offset_t;

//! File size type
typedef unsigned int file_size_t;
typedef unsigned long long big_file_size_t;

//! Counter type
typedef unsigned int counter_t;

#endif