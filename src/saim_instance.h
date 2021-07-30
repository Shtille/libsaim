/*************************************************************************
* libsaim 1.0
* ------------------------------------------------------------------------
*  Copyright (c) 2016 Vladimir Sviridov <v.shtille@gmail.com>
* 
*  This software is provided 'as-is', without any express or implied
*  warranty. In no event will the authors be held liable for any damages
*  arising from the use of this software.
* 
*  Permission is granted to anyone to use this software for any purpose,
*  including commercial applications, and to alter it and redistribute it
*  freely, subject to the following restrictions:
* 
*  1. The origin of this software must not be misrepresented; you must not
*     claim that you wrote the original software. If you use this software
*     in a product, an acknowledgment in the product documentation would
*     be appreciated but is not required.
* 
*  2. Altered source versions must be plainly marked as such, and must not
*     be misrepresented as being the original software.
* 
*  3. This notice may not be removed or altered from any source
*     distribution.
* 
**************************************************************************/

#ifndef __SAIM_INSTANCE_H__
#define __SAIM_INSTANCE_H__

#include <stdbool.h>

typedef struct saim_instance saim_instance;
/**
 * Structure holds instance-specific data
 */
struct saim_instance {
	struct saim_provider * provider;			//!< pointer to provider instance
	struct saim_cache * cache;					//!< pointer to cache instance
	struct saim_rasterizer * rasterizer;		//!< pointer to rasterizer instance
	char path[260];								//!< working path
	bool viewport_clipping_enabled;				//!< is viewport clipping enabled
};

bool saim_instance__create(saim_instance * instance);
void saim_instance__destroy(saim_instance * instance);

bool saim_instance__initialize(saim_instance * instance, const char* hash_string, int service_count);
void saim_instance__deinitialize(saim_instance * instance);

bool saim_instance__set_provider(saim_instance * instance, struct saim_provider_info * provider_info, int flags);

void saim_instance__enable_viewport_clipping(saim_instance * instance);
void saim_instance__disable_viewport_clipping(saim_instance * instance);
bool saim_instance__is_viewport_clipping_enabled(saim_instance * instance);

bool saim_instance__is_in_viewport(saim_instance * instance, const struct saim_data_key * key);

#endif