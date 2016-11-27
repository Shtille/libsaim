#include "saim_memory.h"

#include <stdio.h>

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

static int s_number_allocated = 0;

void saim_memory_init()
{
	s_number_allocated = 0;
}
void saim_memory_check()
{
	if (s_number_allocated != 0)
		fprintf(stderr, "Still there are %i objects that haven't been deallocated\r\n", s_number_allocated);
}
int saim_memory_num_allocations()
{
	return s_number_allocated;
}
void * saim_malloc(size_t size, const char* filename, int line)
{
	++s_number_allocated;
	return malloc(size);
}
void * saim_calloc(size_t num, size_t size, const char* filename, int line)
{
	++s_number_allocated;
	return calloc(num ,size);
}
void * saim_realloc(void * what, size_t size, const char* filename, int line)
{
	// temporary not available
	return realloc(what, size);
}
void saim_free(void * what, const char* filename, int line)
{
	--s_number_allocated;
	free(what);
}
char * saim_strdup(const char* str, const char* filename, int line)
{
	++s_number_allocated;
	return strdup(str);
}