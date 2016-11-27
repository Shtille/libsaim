#include "saim.h"

#include "saim_manager.h"

#include <string.h>
#include <stdio.h>

char s_path[260];

int saim_init(const char* path, saim_provider_info * provider_info)
{
	long length;
	char* hash_string;
	// Append path delimeter here
	length = strlen(path);
	if (length > 230)
	{
		fprintf(stderr, "path parameter is too long\n");
		return 1;
	}
	strcpy(s_path, path);
	if (path[length-1] != '/')
	{
		s_path[length] = '/';
		s_path[length+1] = '\0';
	}
	else
		s_path[length] = '\0';

	// Initialize cache
	if (provider_info != NULL)
		hash_string = provider_info->string;
	else
		hash_string = "default";
	if (!saim_manager__initialize_cache(hash_string))
	{
		fprintf(stderr, "cache initialization failed\n");
		return 2;
	}

	// Set provider parameters
	if (!saim_manager__set_provider(provider_info))
	{
		fprintf(stderr, "provider setup failed\n");
		return 3;
	}

	return 0;
}
void saim_cleanup()
{
	// Deinitialize cache
	saim_manager__deinitialize_cache();
}