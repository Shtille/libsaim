#include "saim_provider.h"

#include <stdio.h>
#include <string.h>

bool saim_provider__create(saim_provider * provider)
{
	saim_string_create(&provider->url_format);
	if (mtx_init(&provider->mutex, mtx_plain) == thrd_error)
	{
		fprintf(stderr, "saim: mutex init failed");
		return false;
	}
	return true;
}
void saim_provider__destroy(saim_provider * provider)
{
	mtx_destroy(&provider->mutex);
	saim_string_destroy(&provider->url_format);
}
bool saim_provider__init(saim_provider * provider, saim_provider_info * provider_info)
{
	/* We gonna exchange all {x},{y},{z} key masks on %i to easy make formatted url text */
	const char *src;
	char *dst;
	int ** argument_ptr;
	int num_arguments;
	unsigned int length, l;
	bool found_argument;

	src = provider_info->string;
	length = (unsigned int) strlen(src);
	l = 0;
	if (length <= 9)
	{
		fprintf(stderr, "saim: wrong URL format\n");
		return false;
	}
	saim_string_resize(&provider->url_format, length - 9 + 6 + 1);
	dst = provider->url_format.data;
	dst[length-3] = '\0';
	
	argument_ptr = & provider->first_argument;
	num_arguments = 0;
	do
	{
		if (l+3 <= length && *src == '{' && *(src+2) == '}')
		{
			if (num_arguments == 3)
			{
				fprintf(stderr, "saim: wrong URL format\n");
				return false;
			}
			found_argument = false;
			if (*(src+1) == 'x')
			{
				*argument_ptr = & provider->passed_key_x;
				found_argument = true;
			}
			else if (*(src+1) == 'y')
			{
				*argument_ptr = & provider->passed_key_y;
				found_argument = true;
			}
			else if (*(src+1) == 'z')
			{
				*argument_ptr = & provider->passed_key_z;
				found_argument = true;
			}
			if (found_argument)
			{
				++argument_ptr;
				++num_arguments;
				*dst++ = '%';
				*dst++ = 'i';
				src += 3;
				l += 3;
				continue;
			}
		}
		*dst++ = *src++;
		++l;
	}
	while (l < length);

	if (num_arguments == 3)
	{
		// Finally copy the rest parameters
		provider->min_lod = provider_info->min_lod;
		provider->max_lod = provider_info->max_lod;
		return true;
	}
	else
		return false;
}
void saim_provider__fill_buffer_for_key(saim_provider * provider, const data_key_t * key, char* buffer)
{
	// Make sure that this function is called only from one thread
	mtx_lock(&provider->mutex);
	provider->passed_key_x = data_key_get_x(key);
	provider->passed_key_y = data_key_get_y(key);
	provider->passed_key_z = data_key_get_z(key);
	sprintf(buffer, provider->url_format.data,
		*provider->first_argument, *provider->second_argument, *provider->third_argument);
	mtx_unlock(&provider->mutex);
}