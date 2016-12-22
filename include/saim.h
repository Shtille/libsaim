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

#ifndef __SAIM_H__
#define __SAIM_H__

#include "saim_provider_info.h"

/*! @brief Initialization function
	@return returns 0 on success and not 0 in case of error
*/
int saim_init(const char* path, saim_provider_info * provider_info, int flags);
void saim_cleanup();

void saim_set_target(unsigned char * buffer, int width, int height, int bytes_per_pixel);
/*! Renders saved bitmap onto target surface
@param[in] upper_latitude Latitude of the upper left point of rendered rectangle
TODO: discribe later
*/
int saim_render_aligned(double upper_latitude, double left_longitude, double lower_latitude, double right_longitude);
int saim_render_common(double upper_latitude, double left_longitude, double lower_latitude, double right_longitude, float angle);

#endif