#include "saim_memory.h"

#include "../../deps/tinycthread.h"

#include <stdio.h>

#define SAIM_MEMORY_DEBUG

/*
static unsigned int make_hash(const char* filename, int line)
{
	unsigned int hash = 5381;
	unsigned char * p;
	int c;

	c = sizeof(line);
	p = (unsigned char *)&line;
	while (c > 0)
	{
		hash = ((hash << 5) + hash) ^ *p;
		++p;
		--c;
	}

	while ((c = *filename++))
		hash = ((hash << 5) + hash) ^ c;

	return hash;
}
*/

#ifdef SAIM_MEMORY_DEBUG

static mtx_t s_mutex;
static int s_number_allocated = 0;
static size_t s_allocated_bytes = 0;

#endif

bool saim_memory_init()
{
#ifdef SAIM_MEMORY_DEBUG
	if (mtx_init(&s_mutex, mtx_plain) == thrd_error)
	{
		fprintf(stderr, "saim: mutex init failed");
		return false;
	}
	s_number_allocated = 0;
	s_allocated_bytes = 0;
#endif
	return true;
}
void saim_memory_cleanup()
{
#ifdef SAIM_MEMORY_DEBUG
	saim_memory_check();
	mtx_destroy(&s_mutex);
#endif
}
void saim_memory_check()
{
#ifdef SAIM_MEMORY_DEBUG
	mtx_lock(&s_mutex);
	if (s_number_allocated != 0)
	{
		fprintf(stderr, "Still there are %i objects that haven't been deallocated\r\n", s_number_allocated);
	}
	mtx_unlock(&s_mutex);
#endif
}
int saim_memory_num_allocations()
{
#ifdef SAIM_MEMORY_DEBUG
	volatile int num_allocated;
	mtx_lock(&s_mutex);
	num_allocated = s_number_allocated;
	mtx_unlock(&s_mutex);
	return num_allocated;
#else
	return 0;
#endif
}
void * saim_malloc(size_t size, const char* filename, int line)
{
#ifdef SAIM_MEMORY_DEBUG
	mtx_lock(&s_mutex);
	++s_number_allocated;
	//s_allocated_bytes += size;
	mtx_unlock(&s_mutex);
#endif
	return malloc(size);
}
void * saim_calloc(size_t num, size_t size, const char* filename, int line)
{
#ifdef SAIM_MEMORY_DEBUG
	mtx_lock(&s_mutex);
	++s_number_allocated;
	//s_allocated_bytes += num * size;
	mtx_unlock(&s_mutex);
#endif
	return calloc(num ,size);
}
void * saim_realloc(void * what, size_t size, const char* filename, int line)
{
#ifdef SAIM_MEMORY_DEBUG
	mtx_lock(&s_mutex);
	// num allocated stays the same
	//s_allocated_bytes -= old_size;
	//s_allocated_bytes += size;
	mtx_unlock(&s_mutex);
#endif
	return realloc(what, size);
}
void saim_free(void * what, const char* filename, int line)
{
#ifdef SAIM_MEMORY_DEBUG
	mtx_lock(&s_mutex);
	--s_number_allocated;
	//s_allocated_bytes -= old_size;
	mtx_unlock(&s_mutex);
#endif
	free(what);
}
char * saim_strdup(const char* str, const char* filename, int line)
{
#ifdef SAIM_MEMORY_DEBUG
	mtx_lock(&s_mutex);
	++s_number_allocated;
	//s_allocated_bytes += strlen(str);
	mtx_unlock(&s_mutex);
#endif
	return strdup(str);
}