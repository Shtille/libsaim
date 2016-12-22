#ifndef __SAIM_MANAGER_H__
#define __SAIM_MANAGER_H__

#include "saim_data_key.h"
#include "saim_cache.h"
#include "saim_provider_info.h"

#include <stdbool.h>

bool saim_manager__initialize_cache(const char* hash_string);
void saim_manager__deinitialize_cache();

bool saim_manager__set_provider(saim_provider_info * provider_info, int flags);

saim_cache * saim_manager__get_cache();

void saim_manager__enable_viewport_clipping();
void saim_manager__disable_viewport_clipping();
bool saim_manager__is_viewport_clipping_enabled();

bool saim_manager__is_in_viewport(const saim_data_key * key);

#endif