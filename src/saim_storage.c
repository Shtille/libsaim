#include "saim_storage.h"

#include "util/saim_memory.h"
#include "util/saim_file_op.h"

#include <string.h>

static const file_offset_t kMaxMainFileSize = 300 << 20; // 300 MB
static const file_offset_t kMaxRegionFileSize = 500 << 20; // 500 MB

extern char s_path[260]; // declared in saim_init.c

static void append_path(char * buffer, const char* filename)
{
	size_t path_length, filename_length;
	path_length = strlen(s_path);
	filename_length = strlen(filename);
	strncpy(buffer, s_path, path_length);
	strncpy(buffer + path_length, filename, filename_length);
	buffer[path_length + filename_length] = '\0';
}

bool storage_create(storage_t * storage, const char* hash_string)
{
	if (mtx_init(&storage->critical_section, mtx_plain) == thrd_error)
	{
		fprintf(stderr, "saim: mutex init failed\n");
		return false;
	}
	if (mtx_init(&storage->critical_section_key_set, mtx_plain) == thrd_error)
	{
		fprintf(stderr, "saim: mutex init failed\n");
		return false;
	}
	storage->hash_value = saim_hash(hash_string);
	key_set_create(&storage->key_set);
	region_map_create(&storage->region_map);
	storage_info_map_create(&storage->region_info_map);
	return true;
}
void storage_destroy(storage_t * storage)
{
	storage_deinitialize(storage);
	storage_info_map_destroy(&storage->region_info_map);
	region_map_destroy(&storage->region_map);
	key_set_destroy(&storage->key_set);
	mtx_destroy(&storage->critical_section_key_set);
	mtx_destroy(&storage->critical_section);
}
bool storage_initialize(storage_t * storage)
{
	char buffer[260];
	saim_set_node *node, *nil;
	region_map_pair_t * pair;
	storage_info_pair_t * info_pair;
	initialize_result_t result;

	append_path(buffer, "regions.omrh");
	regions_header_file_create(&storage->regions_header_file, buffer);
	append_path(buffer, "data.om");
	storage_info_create(&storage->main_info);
	saim_storage_file_create(&storage->main_info.file, buffer, storage->hash_value, kMaxMainFileSize);
	if (storage_initialize_file(storage, &storage->main_info) == kStorage_Failed)
		return false;
	if (!storage_initialize_regions_header_file(storage))
		return false;
	// Initialize region files
	nil = storage->region_map.set->nil;
	node = saim_set_get_first(storage->region_map.set);
	while (node != nil)
	{
		pair = (region_map_pair_t *)node->data;
		info_pair = SAIM_MALLOC(sizeof(storage_info_pair_t));
		storage_info_pair_create(info_pair);
		saim_string_set(&info_pair->name, &pair->name);
		saim_storage_file_create(&info_pair->info.file, pair->info.filename.data, storage->hash_value, kMaxRegionFileSize);
		storage_info_map_insert(&storage->region_info_map, info_pair);
		result = storage_initialize_file(storage, &info_pair->info);
		if (result == kStorage_Failed) // offsets & list
			return false;
		if (result == kStorage_Regenerated) // region storage file has been corrupted
		{
			fprintf(stderr, "saim: region '%s' storage file has been broken\n", pair->name.data);
			pair->info.status = kRegion_Invalid;
			// Update header file information
			if (!regions_header_file_update(&storage->regions_header_file, &storage->region_map))
				return false;
		}
		if (pair->info.status == kRegion_Invalid)
		{
			fprintf(stderr, "saim: region '%s' is invalid\n", pair->name.data);
		}
		node = saim_set_get_next(storage->region_map.set, node);
	}
	storage_initialize_key_set(storage);
	return true;
}
key_pair_t * storage_get_key_pair(storage_t * storage, const data_key_t * key, storage_info_t ** storage_info)
{
	saim_set_node *node, *nil, *node2, *nil2;
	storage_info_pair_t * info_pair;
	key_pair_t * return_pair = 0;

	mtx_lock(&storage->critical_section);
	// First time search in the main storage file
	nil = storage->main_info.offsets.set->nil;
	node = key_offset_map_search(&storage->main_info.offsets, key);
	if (node != nil)
	{
		*storage_info = &storage->main_info; // specify main data source
		return_pair = (key_pair_t *)node->data;
	}
	else // Try to find this key in regions
	{
		nil = storage->region_info_map.set->nil;
    	node = saim_set_get_first(storage->region_info_map.set);
    	while (node != nil)
    	{
    		info_pair = (storage_info_pair_t *)node->data;
    		nil2 = info_pair->info.offsets.set->nil;
    		node2 = key_offset_map_search(&info_pair->info.offsets, key);
    		if (node2 != nil2)
    		{
    			*storage_info = &info_pair->info; // specify region data source
    			return_pair = (key_pair_t *)node2->data;
    			break;
    		}
    		node = saim_set_get_next(storage->region_info_map.set, node);
    	}
	}
	mtx_unlock(&storage->critical_section);
	return return_pair;
}
bool storage_is_exist(storage_t * storage, const data_key_t * key)
{
	saim_set_node * node;
	bool is_exist;

	mtx_lock(&storage->critical_section_key_set);
	node = saim_set_search(storage->key_set.set, key);
	is_exist = (node != storage->key_set.set->nil);
	mtx_unlock(&storage->critical_section_key_set);
	return is_exist;
}
bool storage_load(storage_t * storage, key_pair_t * pair, saim_string * data, storage_info_t * info)
{
	saim_file * base_file = &info->file.file;
	stored_key_pair_t stored_pair;
	bool result = false;

	mtx_lock(&storage->critical_section);
	if (saim_file_open_for_read(base_file))
	{
		// Increase counter of found element in the map
		++pair->info.counter;
		// List contains iterators to map, so we just should sort it
		key_list_sort(&info->list);
		// We should update counter value in the file too
		make_stored_key_pair(&stored_pair, pair);
		saim_file_offset_from_beginning(base_file, pair->info.key_offset);
		saim_storage_file_write_key_pair(&info->file, &stored_pair);
		// Offset to data
		saim_file_offset_from_beginning(base_file, pair->info.data_offset);
		// Allocate space for data
		saim_string_resize(data, pair->info.size);
		// Read the data
		saim_file_read_array(base_file, data->data, pair->info.size);
		// Finally
		saim_file_close(base_file);
		result = true;
	}
	mtx_unlock(&storage->critical_section);
	return result;
}
bool storage_save_main(storage_t * storage, const data_key_t * key, const saim_string * data)
{
	bool result;
	mtx_lock(&storage->critical_section);
	result = storage_save(storage, key, data, &storage->main_info);
	mtx_unlock(&storage->critical_section);
	return result;
}
bool storage_save_separate(storage_t * storage, const data_key_t * key, const saim_string * data, const saim_string * name)
{
	bool result;
	saim_set_node * node;
	storage_info_pair_t * pair;

	mtx_lock(&storage->critical_section);
	node = storage_info_map_search(&storage->region_info_map, name);
	if (node == storage->region_info_map.set->nil)
	{
		fprintf(stderr, "saim: did not find region storage with name '%s'\n", name->data);
		result = false;
	}
	else // normal case
	{
		pair = (storage_info_pair_t *)node->data;
		result = storage_save(storage, key, data, &pair->info);
	}
	mtx_unlock(&storage->critical_section);
	return result;
}
bool storage_region_add(storage_t * storage, const region_info_t * region_info)
{
	char buffer[260];
	saim_set_node * node;
	storage_info_pair_t * info_pair;
	region_map_pair_t * region_pair;

	mtx_lock(&storage->critical_section);

	node = region_map_search(&storage->region_map, &region_info->name);
	if (node != storage->region_map.set->nil)
	{
		fprintf(stderr, "saim: region '%s' already exists\n", region_info->name.data);
		mtx_unlock(&storage->critical_section);
        return false; // already exists
	}
	// Generate a new filename
	storage_generate_region_file_name(storage, buffer);

	// Add a new object to map
    region_pair = (region_map_pair_t *)SAIM_MALLOC(sizeof(region_map_pair_t));
    region_map_pair_create(region_pair);
    saim_string_set(&region_pair->name, &region_info->name);
    (void)region_map_insert(&storage->region_map, region_pair);
    region_info_copy(&region_pair->info.info, region_info);
    region_pair->info.status = kRegion_Downloading;
    region_pair->info.time = time(NULL); // use current time
    saim_string_set_text(&region_pair->info.filename, buffer);

	// Add file to region info map
	info_pair = (storage_info_pair_t *)SAIM_MALLOC(sizeof(storage_info_pair_t));
	storage_info_pair_create(info_pair);
	saim_string_set(&info_pair->name, &region_info->name);
	(void)storage_info_map_insert(&storage->region_info_map, info_pair);

	// Create a new storage file
	saim_storage_file_create(&info_pair->info.file, buffer, storage->hash_value, kMaxRegionFileSize);

	// Create file on disk
	saim_storage_file_regenerate(&info_pair->info.file);

	// Write information into header file
    if (!regions_header_file_add_region(&storage->regions_header_file, &storage->region_map))
    {
    	mtx_unlock(&storage->critical_section);
        return false;
    }

	mtx_unlock(&storage->critical_section);

	return true;
}
bool storage_region_rename(storage_t * storage, const saim_string * old_name, const saim_string * new_name)
{
	saim_set_node * node;
	region_map_pair_t *region_pair;
	storage_info_pair_t * info_pair;

	mtx_lock(&storage->critical_section);

	node = region_map_search(&storage->region_map, old_name);
	if (node == storage->region_map.set->nil)
	{
		fprintf(stderr, "saim: region '%s' hasn't been found\n", old_name->data);
		mtx_unlock(&storage->critical_section);
        return false; // didn't find old one
	}

	// Change map object (name is also a map key, so we have to erase element)
	region_pair = (region_map_pair_t *)node->data;
	region_map_cut(&storage->region_map, node);
	saim_string_set(&region_pair->name, new_name);
	saim_string_set(&region_pair->info.info.name, new_name);
	(void)region_map_insert(&storage->region_map, region_pair);

	// Also need to change files map
	node = storage_info_map_search(&storage->region_info_map, old_name);
	info_pair = (storage_info_pair_t *)node->data;
	storage_info_map_cut(&storage->region_info_map, node);
	saim_string_set(&info_pair->name, new_name);
	(void)storage_info_map_insert(&storage->region_info_map, info_pair);

	// Update header file information
	if (!regions_header_file_update(&storage->regions_header_file, &storage->region_map))
	{
		mtx_unlock(&storage->critical_section);
		return false;
	}

	mtx_unlock(&storage->critical_section);

	return true;
}
bool storage_region_delete(storage_t * storage, const saim_string * name)
{
	saim_set_node * node;
	storage_info_pair_t * info_pair;

	mtx_lock(&storage->critical_section);

	node = region_map_search(&storage->region_map, name);
	if (node == storage->region_map.set->nil)
	{
		fprintf(stderr, "saim: region '%s' hasn't been found\n", name->data);
		mtx_unlock(&storage->critical_section);
        return false; // didn't find old one
	}

	// Remove region from map
	region_map_erase(&storage->region_map, node);

	// Remove region file from list
	node = storage_info_map_search(&storage->region_info_map, name);
	info_pair = (storage_info_pair_t *)node->data;

	// Delete file from disk
	saim_file_op_delete(info_pair->info.file.file.filename);

	saim_storage_file_destroy(&info_pair->info.file);
	storage_info_map_erase(&storage->region_info_map, node);

	// Need to reinitialize key set
	mtx_lock(&storage->critical_section_key_set);
	storage_initialize_key_set(storage);
	mtx_unlock(&storage->critical_section_key_set);

	// Update header file information
	if (!regions_header_file_remove_region(&storage->regions_header_file, &storage->region_map))
	{
		mtx_unlock(&storage->critical_section);
		return false;
	}

	mtx_unlock(&storage->critical_section);

	return true;
}
bool storage_region_mark_stored(storage_t * storage, const saim_string * name)
{
	saim_set_node * node;
	region_map_pair_t * region_pair;

	mtx_lock(&storage->critical_section);

	node = region_map_search(&storage->region_map, name);
	if (node == storage->region_map.set->nil)
	{
		fprintf(stderr, "saim: region '%s' hasn't been found\n", name->data);
		mtx_unlock(&storage->critical_section);
        return false; // didn't find one
	}

	// Update map data
	region_pair = (region_map_pair_t *)node->data;
	region_pair->info.status = kRegion_Stored;

	// Update header file information
	if (!regions_header_file_update(&storage->regions_header_file, &storage->region_map))
	{
		mtx_unlock(&storage->critical_section);
		return false;
	}

	mtx_unlock(&storage->critical_section);

	return true;
}
bool storage_region_mark_invalid(storage_t * storage, const saim_string * name)
{
	saim_set_node * node;
	region_map_pair_t * region_pair;

	mtx_lock(&storage->critical_section);

	node = region_map_search(&storage->region_map, name);
	if (node == storage->region_map.set->nil)
	{
		fprintf(stderr, "saim: region '%s' hasn't been found\n", name->data);
		mtx_unlock(&storage->critical_section);
        return false; // didn't find one
	}

	// Update map data
	region_pair = (region_map_pair_t *)node->data;
	region_pair->info.status = kRegion_Invalid;

	// Update header file information
	if (!regions_header_file_update(&storage->regions_header_file, &storage->region_map))
	{
		mtx_unlock(&storage->critical_section);
		return false;
	}

	mtx_unlock(&storage->critical_section);

	return true;
}
bool storage_get_region_info(storage_t * storage, const saim_string * name, stored_region_info_t ** info)
{
	bool result;
	saim_set_node * node;
	region_map_pair_t * pair;

	mtx_lock(&storage->critical_section);
	node = region_map_search(&storage->region_map, name);
	if (node == storage->region_map.set->nil)
	{
		result = false;
	}
	else // normal case
	{
		pair = (region_map_pair_t *)node->data;
		*info = &pair->info;
		result = true;
	}
	mtx_unlock(&storage->critical_section);
	return result;
}
big_file_size_t storage_get_region_file_size(storage_t * storage, const saim_string * name)
{
	return 0;
}
unsigned int storage_get_key_count(storage_t * storage)
{
	unsigned int count;
	mtx_lock(&storage->critical_section_key_set);
	count = key_set_size(&storage->key_set);
	mtx_unlock(&storage->critical_section_key_set);
	return count;
}
big_file_size_t storage_get_estimated_file_size(unsigned int num_tiles, unsigned int data_size)
{
	return saim_storage_file_get_estimated_file_size(num_tiles, data_size);
}
big_file_size_t storage_get_maximum_region_file_size()
{
	return (big_file_size_t)kMaxRegionFileSize;
}
initialize_result_t storage_initialize_file(storage_t * storage, storage_info_t * info)
{
	saim_file * base_file = &info->file.file;
	 // Read storage setting from header and initialize the key map
	if (saim_file_open_for_read(base_file))
	{
	    if (saim_storage_file_read_header(&info->file))
	    {
	        if (!base_file->operation_successful)
	        {
	        	saim_file_close(base_file);
	            return kStorage_Failed;
	        }
	        // Initialize key map
	        saim_storage_file_read_all_keys(&info->file, &info->offsets, &info->list);
	        saim_file_close(base_file);
	        return kStorage_Success;
	    }
	    else if (base_file->operation_successful)
	    {
	        // The file has been regenerated
	        saim_file_close(base_file);
	        return kStorage_Regenerated;
	    }
	    else
	    {
	        // File regeneration has been failed
	        saim_file_close(base_file);
	        return kStorage_Failed;
	    }
	}
	else if (saim_file_open_for_write(base_file))
	{
	    fprintf(stdout, "saim: storage initialization\n");
	    // Seems like file doesn't exist, let's create a new one
	    saim_storage_file_write_header(&info->file);
	    if (!base_file->operation_successful)
	    {
	        saim_file_close(base_file);
	        return kStorage_Failed;
	    }
	    // There are no data yet, so we won't fill our map.
	    saim_file_close(base_file);
	    return kStorage_Success;
	}
	else
	    return kStorage_Failed;
}
bool storage_initialize_regions_header_file(storage_t * storage)
{
	saim_file * base_file = &storage->regions_header_file.file;
	// Read storage setting from header and initialize the key map
    if (saim_file_open_for_read(base_file))
    {
        if (regions_header_file_read_header(&storage->regions_header_file))
        {
            if (!base_file->operation_successful)
            {
                saim_file_close(base_file);
                return false;
            }
            // Initialize key map
            regions_header_file_read_all_regions(&storage->regions_header_file, &storage->region_map);
            saim_file_close(base_file);
            return true;
        }
        else if (base_file->operation_successful)
        {
            // The file has been regenerated
            saim_file_close(base_file);
            return true;
        }
        else
        {
            // File regeneration has been failed
            saim_file_close(base_file);
            return false;
        }
    }
    else if (saim_file_open_for_write(base_file))
    {
        fprintf(stdout, "saim: regions header file initialization\n");
        // Seems like file doesn't exist, let's create a new one
        regions_header_file_write_header(&storage->regions_header_file);
        if (!base_file->operation_successful)
        {
            saim_file_close(base_file);
            return false;
        }
        // There are no data yet, so we won't fill our map.
        saim_file_close(base_file);
        return true;
    }
    else
        return false;
}
void storage_initialize_key_set(storage_t * storage)
{
	saim_set_node *node, *node2;
	saim_set_node *nil, *nil2;
	key_pair_t * pair;
	data_key_t * key_copy;
	storage_info_pair_t * info_pair;

	key_set_clear(&storage->key_set);
    // From main storage
    nil = storage->main_info.offsets.set->nil;
    node = saim_set_get_first(storage->main_info.offsets.set);
    while (node != nil)
    {
    	pair = (key_pair_t *)node->data;
		key_copy = (data_key_t *)SAIM_MALLOC(sizeof(data_key_t));
		data_key_set_by_other(key_copy, &pair->key);
    	key_set_insert(&storage->key_set, key_copy);
    	node = saim_set_get_next(storage->main_info.offsets.set, node);
    }
    // From regions storages
    nil = storage->region_info_map.set->nil;
    node = saim_set_get_first(storage->region_info_map.set);
    while (node != nil)
    {
    	info_pair = (storage_info_pair_t *)node->data;

    	nil2 = info_pair->info.offsets.set->nil;
    	node2 = saim_set_get_first(info_pair->info.offsets.set);
    	while (node2 != nil2)
    	{
    		pair = (key_pair_t *)node2->data;
			key_copy = (data_key_t *)SAIM_MALLOC(sizeof(data_key_t));
			data_key_set_by_other(key_copy, &pair->key);
    		key_set_insert(&storage->key_set, key_copy);
    		node2 = saim_set_get_next(info_pair->info.offsets.set, node2);
    	}
    	node = saim_set_get_next(storage->region_info_map.set, node);
    }
}
void storage_deinitialize_regions(storage_t * storage)
{
	saim_set_node *node, *nil;
	storage_info_pair_t * info_pair;

	region_map_clear(&storage->region_map);
	nil = storage->region_info_map.set->nil;
    node = saim_set_get_first(storage->region_info_map.set);
    while (node != nil)
    {
    	info_pair = (storage_info_pair_t *)node->data;
    	saim_storage_file_destroy(&info_pair->info.file);
    	node = saim_set_get_next(storage->region_info_map.set, node);
    }
    storage_info_map_clear(&storage->region_info_map);
}
void storage_deinitialize(storage_t * storage)
{
	storage_deinitialize_regions(storage);
	saim_storage_file_destroy(&storage->main_info.file);
	storage_info_destroy(&storage->main_info);
	regions_header_file_destroy(&storage->regions_header_file);
}
void storage_generate_region_file_name(storage_t * storage, char* buffer)
{
	unsigned int id;
	char filename[30];

	id = storage->regions_header_file.unique_id;
	sprintf(filename, "%u.omr", id);
	append_path(buffer, filename);
}
bool storage_save(storage_t * storage, const data_key_t * key, const saim_string * data, storage_info_t * info)
{
	saim_file * base_file = &info->file.file;
	save_result_t result;
	data_key_t * key_copy;
	bool need_to_flush;

	if (data->length == 0)
	{
		fprintf(stderr, "saim: curl returned empty data for key(%i,%i,%i)\n",
                      data_key_get_z(key), data_key_get_x(key), data_key_get_y(key));
		return true;
	}
	if (saim_file_open_for_read(base_file))
	{
		// We will protect all write operations to avoid data corruption
		// (application may crash due to external reason, a.e.)
		saim_storage_file_mark_operations_begin(&info->file);
		// The main save logics is done here
		result = saim_storage_file_save_logics(&info->file, key, data, &info->offsets, &info->list);
		saim_storage_file_mark_operations_end(&info->file);
		// Finally
		saim_file_close(base_file);
		need_to_flush = false;
		if (result == kSave_NeedToFlush)
		{
			fprintf(stdout, "saim: storage flush due to reaching hard cap\n");
			if (saim_storage_file_flush(&info->file))
			{
				need_to_flush = true;
				key_offset_map_clear(&info->offsets);
				key_list_clear(&info->list);
			}
		}
		mtx_lock(&storage->critical_section_key_set);
		if (need_to_flush)
			storage_initialize_key_set(storage);
		else
		{
			key_copy = (data_key_t *)SAIM_MALLOC(sizeof(data_key_t));
			data_key_set_by_other(key_copy, key);
			key_set_insert(&storage->key_set, key_copy);
		}
		mtx_unlock(&storage->critical_section_key_set);
		return result == kSave_Success;
	}
	else
		return false;
}