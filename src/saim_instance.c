#include "saim_instance.h"

#include "saim_data_key.h"
#include "saim_key_region.h"
#include "saim_provider.h"
#include "saim_cache.h"
#include "rasterizer/saim_rasterizer.h"
#include "rasterizer/saim_rasterizer_async.h"

#include "util/saim_memory.h"
#include "util/saim_string.h"

bool saim_instance__create(saim_instance * instance)
{
	instance->provider = NULL;
	instance->cache = NULL;
	instance->rasterizer = NULL;
	instance->rasterizer_async = NULL;
	instance->viewport_clipping_enabled = false;
	return true;
}
void saim_instance__destroy(saim_instance * instance)
{

}
bool saim_instance__initialize(saim_instance * instance, const char* hash_string, int service_count)
{
	if (!saim_memory_init())
		return false;

	instance->provider = (saim_provider *) SAIM_MALLOC(sizeof(saim_provider));
	if (!saim_provider__create(instance->provider))
		return false;

	instance->rasterizer = (saim_rasterizer *) SAIM_MALLOC(sizeof(saim_rasterizer));
	if (!saim_rasterizer__create(instance->rasterizer, instance))
		return false;

	instance->rasterizer_async = (saim_rasterizer_async *) SAIM_MALLOC(sizeof(saim_rasterizer_async));
	if (!saim_rasterizer_async__create(instance->rasterizer_async, instance))
		return false;

	instance->cache = (saim_cache *) SAIM_MALLOC(sizeof(saim_cache));
	if (!saim_cache__create(instance->cache, hash_string, service_count, instance))
		return false;
	if (!saim_cache__initialize(instance->cache))
		return false;

	return true;
}
void saim_instance__deinitialize(saim_instance * instance)
{
	if (instance->cache)
	{
		saim_cache__destroy(instance->cache);
		SAIM_FREE(instance->cache);
		instance->cache = NULL;
	}
	// Rasterizer should be deinitialized after cache due to notification calls
	if (instance->rasterizer)
	{
		saim_rasterizer__destroy(instance->rasterizer);
		SAIM_FREE(instance->rasterizer);
		instance->rasterizer = NULL;
	}
	if (instance->rasterizer_async)
	{
		saim_rasterizer_async__destroy(instance->rasterizer_async);
		SAIM_FREE(instance->rasterizer_async);
		instance->rasterizer_async = NULL;
	}
	if (instance->provider)
	{
		saim_provider__destroy(instance->provider);
		SAIM_FREE(instance->provider);
		instance->provider = NULL;
	}
	saim_memory_cleanup();
}
bool saim_instance__set_provider(saim_instance * instance, saim_provider_info * provider_info, int flags)
{
	const char * kDefaultUrlMask = "http://services.arcgisonline.com/arcgis/rest/services/World_Imagery/MapServer/tile/{z}/{y}/{x}";
	saim_provider_info default_provider_info;
	if (provider_info) // user defined provider info
		memcpy(&default_provider_info, provider_info, sizeof(saim_provider_info));
	else // default provider info
	{
		strcpy(default_provider_info.string, kDefaultUrlMask);
		default_provider_info.string[strlen(kDefaultUrlMask)] = '\0';
	}
	return saim_provider__init(instance->provider, &default_provider_info, flags);
}
void saim_instance__enable_viewport_clipping(saim_instance * instance)
{
	instance->viewport_clipping_enabled = true;
}
void saim_instance__disable_viewport_clipping(saim_instance * instance)
{
	instance->viewport_clipping_enabled = false;
}
bool saim_instance__is_viewport_clipping_enabled(saim_instance * instance)
{
	return instance->viewport_clipping_enabled;
}
bool saim_instance__is_in_viewport(saim_instance * instance, const saim_data_key * key)
{
	if (!instance->viewport_clipping_enabled)
		return true;
	// TODO: add regions check
	return false;
}