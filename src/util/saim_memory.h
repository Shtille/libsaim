#ifndef __SAIM_MEMORY__
#define __SAIM_MEMORY__

/*
* Debug versions of memory allocation and freeing fucntions
*/

#include <stdlib.h>
#include <string.h>

void saim_memory_init();
void saim_memory_check();
int saim_memory_num_allocations();

void * saim_malloc(size_t size, const char* filename, int line);
void * saim_calloc(size_t num, size_t size, const char* filename, int line);
void * saim_realloc(void * what, size_t size, const char* filename, int line);
void saim_free(void * what, const char* filename, int line);
char * saim_strdup(const char* str, const char* filename, int line);

#ifndef NDEBUG // debug

#define SAIM_MALLOC(size)		saim_malloc(size, __FILE__, __LINE__)
#define SAIM_CALLOC(num,size)	saim_calloc(num, size, __FILE__, __LINE__)
#define SAIM_REALLOC(what,size)	saim_realloc(what, size, __FILE__, __LINE__)
#define SAIM_FREE(what)			saim_free(what, __FILE__, __LINE__)

#define SAIM_STRDUP(str)		saim_strdup(str, __FILE__, __LINE__)

#else // release

#define SAIM_MALLOC(size)		malloc(size)
#define SAIM_CALLOC(num,size)	calloc(num,size)
#define SAIM_REALLOC(what,size)	realloc(what,size)
#define SAIM_FREE(what)			free(what)

#define SAIM_STRDUP(str)		strdup(str)

#endif // NDEBUG

#endif