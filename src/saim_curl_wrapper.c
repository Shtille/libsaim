#include "saim_curl_wrapper.h"

#include "curl/curl.h"

#include <stdio.h>

bool saim_curl_wrapper__create(saim_curl_wrapper_t * curlw)
{
	curlw->curl = curl_easy_init();
	if (!curlw->curl)
	{
		fprintf(stderr, "saim: failed to init libcurl\n");
        return false;
	}
    return true;
}
void saim_curl_wrapper__destroy(saim_curl_wrapper_t * curlw)
{
	if (curlw->curl)
		curl_easy_cleanup(curlw->curl);
}
bool saim_curl_wrapper__download(saim_curl_wrapper_t * curlw, const char* url, void* userdata, CurlWriteFunction func)
{
	void* curl = curlw->curl;

	if (curl)
    {
        CURLcode res;
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, userdata);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, func);
        res = curl_easy_perform(curl);
        if (CURLE_OK == res)
            return true;
        else
        {
            fprintf(stderr, "saim: curl_easy_perform error %i\n", res);
            return false;
        }
    }
    else
        return false;
}