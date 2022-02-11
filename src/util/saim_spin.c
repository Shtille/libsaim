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

#include "saim_spin.h"

#include "../../deps/tinycthread.h"

void saim_spin__init(saim_spin * spin)
{
	atomic_init(&spin->locked, false);
}
void saim_spin__lock(saim_spin * spin)
{
	for (;;)
	{
		// Optimistically assume the lock is free on the first try
		if (!atomic_exchange_explicit(&spin->locked, true, memory_order_acquire))
			return;

		// Wait for lock to be released without generating cache misses
		while (atomic_load_explicit(&spin->locked, memory_order_relaxed))
		{
			// Issue X86 PAUSE or ARM YIELD instruction to reduce contention between
			// hyper-threads
			thrd_yield();
		}
	}
}
bool saim_spin__try_lock(saim_spin * spin)
{
	// First do a relaxed load to check if lock is free in order to prevent
	// unnecessary cache misses if someone does while(!try_lock())
	return !atomic_load_explicit(&spin->locked, memory_order_relaxed) &&
	       !atomic_exchange_explicit(&spin->locked, true, memory_order_acquire);
}
void saim_spin__unlock(saim_spin * spin)
{
	atomic_store_explicit(&spin->locked, false, memory_order_release);
}