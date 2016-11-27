#ifndef __SAIM_DATA_INFO_H__
#define __SAIM_DATA_INFO_H__

#include "saim_defines.h"

#pragma pack(push, 1)

//! Data information to store in file
typedef struct {
    file_offset_t offset;   //!< offset from the beginning of file
    file_size_t size;       //!< size of stored data
    counter_t counter;      //!< usage counter for key replacement
} stored_data_info_t;

//! Data information struct for inner use
typedef struct {
    file_offset_t key_offset;   //!< key offset from the beginning of file
    file_offset_t data_offset;  //!< data offset from the beginning of file
    file_size_t size;           //!< size of stored data
    counter_t counter;          //!< usage counter for key replacement
} data_info_t;

#pragma pack(pop)

#endif