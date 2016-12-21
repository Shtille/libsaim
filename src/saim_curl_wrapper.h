#ifndef __SAIM_CURL_WRAPPER_H__
#define __SAIM_CURL_WRAPPER_H__

#include <stddef.h>
#include <stdbool.h>

// Forward declaration
typedef struct saim_curl_wrapper saim_curl_wrapper;

struct saim_curl_wrapper {
	void* curl;
};

/*! Curl write function declaration
@param buffer
@param size
@param nmemb
@param userdata
*/
typedef size_t (*CurlWriteFunction)(void*, size_t, size_t, void*);

bool saim_curl_wrapper__create(saim_curl_wrapper * curlw);
void saim_curl_wrapper__destroy(saim_curl_wrapper * curlw);

bool saim_curl_wrapper__download(saim_curl_wrapper * curlw, const char* url, void* userdata, CurlWriteFunction func);

#endif