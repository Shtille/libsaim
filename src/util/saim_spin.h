/*************************************************************************
* libsaim 1.0
* ------------------------------------------------------------------------
*  Copyright (c) 2022 Vladimir Sviridov <v.shtille@gmail.com>
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

#ifndef __SAIM_SPIN_H__
#define __SAIM_SPIN_H__

#include <stdbool.h>
#include <stdatomic.h>

// Based on https://rigtorp.se/spinlock/
typedef struct {
	atomic_bool locked;
} saim_spin;

void saim_spin__init(saim_spin * spin);
void saim_spin__lock(saim_spin * spin);
bool saim_spin__try_lock(saim_spin * spin);
void saim_spin__unlock(saim_spin * spin);

#endif