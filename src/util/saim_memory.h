#ifndef __SAIM_MEMORY_H__
#define __SAIM_MEMORY_H__

/*
* Debug versions of memory allocation and freeing functions
*/

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

bool saim_memory_init();
void saim_memory_cleanup();

void saim_memory_check();
int saim_memory_num_allocations();

void * saim_malloc(size_t size, const char* filename, int line);
void * saim_calloc(size_t num, size_t size, const char* filename, int line);
void * saim_realloc(void * what, size_t size, const char* filename, int line);
void saim_free(void * what);
char * saim_strdup(const char* str, const char* filename, int line);

#ifndef NDEBUG // debug

#ifdef _MSC_VER // Visual C++ compiler

#include <crtdbg.h>

// Visual C++ has its own memory functions
#define SAIM_VISUAL_STUDIO_MEMORY_DEBUG

#define SAIM_MALLOC(size)		_malloc_dbg(size, _NORMAL_BLOCK, __FILE__, __LINE__)
#define SAIM_CALLOC(num,size)	_calloc_dbg(num, size, _NORMAL_BLOCK, __FILE__, __LINE__)
#define SAIM_REALLOC(what,size)	_realloc_dbg(what, size, _NORMAL_BLOCK, __FILE__, __LINE__)
#define SAIM_FREE(what)			_free_dbg(what, _NORMAL_BLOCK)

#define SAIM_STRDUP(str)		_strdup_dbg(str, _NORMAL_BLOCK, __FILE__, __LINE__)

#else // other compiler

// Use our functions
#define SAIM_MEMORY_DEBUG

#define SAIM_MALLOC(size)		saim_malloc(size, __FILE__, __LINE__)
#define SAIM_CALLOC(num,size)	saim_calloc(num, size, __FILE__, __LINE__)
#define SAIM_REALLOC(what,size)	saim_realloc(what, size, __FILE__, __LINE__)
#define SAIM_FREE(what)			saim_free(what)

#define SAIM_STRDUP(str)		saim_strdup(str, __FILE__, __LINE__)

#endif // _MSC_VER

#else // release

#define SAIM_MALLOC(size)		malloc(size)
#define SAIM_CALLOC(num,size)	calloc(num,size)
#define SAIM_REALLOC(what,size)	realloc(what,size)
#define SAIM_FREE(what)			free(what)

#define SAIM_STRDUP(str)		strdup(str)

#endif // NDEBUG

#endif