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

#ifndef __SAIM_BITMAP_INFO_PAIR_H__
#define __SAIM_BITMAP_INFO_PAIR_H__

#include "../saim_data_key.h"
#include "saim_bitmap_info.h"

typedef struct {
	saim_data_key key;
	saim_bitmap_info info;
} saim_bitmap_info_pair;

void saim_bitmap_info_pair__create(saim_bitmap_info_pair * info_pair);
void saim_bitmap_info_pair__destroy(saim_bitmap_info_pair * info_pair);

int saim_bitmap_info_pair__compare(const saim_bitmap_info_pair* p1, const saim_bitmap_info_pair* p2);

#endif