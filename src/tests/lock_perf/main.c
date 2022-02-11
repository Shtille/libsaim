/*************************************************************************
*  Test program for multithreaded vs single threaded performance test.
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

#include "../../../deps/tinycthread.h" // for thread routines
#include "../../util/saim_spin.h"

#include <stdio.h>
#include <time.h>

const int kCount = 100000;
const int kNumThreads = 2;

static int s_counter = 0;
static mtx_t s_mutex;
static saim_spin s_spin;

void mutex_test()
{
	for (int i = 0; i < kCount; ++i)
	{
		mtx_lock(&s_mutex);
		++s_counter;
		mtx_unlock(&s_mutex);
	}
}
void spin_test()
{
	for (int i = 0; i < kCount; ++i)
	{
		saim_spin__lock(&s_spin);
		++s_counter;
		saim_spin__unlock(&s_spin);
	}
}

int main()
{
	clock_t start, end;
	double cpu_time_used;

	// Init resources
	if (mtx_init(&s_mutex, mtx_plain) != thrd_success)
		return 1;
	saim_spin__init(&s_spin);

	start = clock();
	mutex_test();
	end = clock();
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("mutex test took %lf seconds\n", cpu_time_used);

	start = clock();
	spin_test();
	end = clock();
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("spin test took %lf seconds\n", cpu_time_used);

	// Release resources
	mtx_destroy(&s_mutex);

	return 0;
}