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