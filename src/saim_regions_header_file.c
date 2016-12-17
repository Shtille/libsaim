#include "saim_regions_header_file.h"

#include "util/saim_memory.h"

#include <assert.h>

static const unsigned int kFormatSignature = 1213353295;

/*
File version changes:
Version 1.
Original version.
*/
static const unsigned char kVersion = 1;

// static const file_offset_t kHeaderSize =
// 	sizeof(unsigned int)  // signature
//   + sizeof(unsigned char) // version
//   + sizeof(unsigned char) // corruption
//   + sizeof(unsigned int)  // unique ID
//   + sizeof(unsigned int); // number of regions
static const file_offset_t kCorruptionByteOffset = sizeof(unsigned int) + sizeof(unsigned char);

void regions_header_file_create(regions_header_file_t * file, const char* filename)
{
	saim_file_create(&file->file, filename);
	file->unique_id = 0U;
	file->num_regions = 0U;
}
void regions_header_file_destroy(regions_header_file_t * file)
{
	saim_file_destroy(&file->file);
}
bool regions_header_file_add_region(regions_header_file_t * file, region_map_t * map)
{
	++file->unique_id; // we need different ID for a new generated name
	++file->num_regions;
	return regions_header_file_update(file, map);
}
bool regions_header_file_remove_region(regions_header_file_t * file, region_map_t * map)
{
	--file->num_regions;
	return regions_header_file_update(file, map);
}
bool regions_header_file_update(regions_header_file_t * file, region_map_t * map)
{
	saim_set_node * node;
	region_map_pair_t * pair;

	saim_file_close(&file->file);
	if (saim_file_open_for_write(&file->file))
	{
		regions_header_file_write_header(file);
		// We will protect all write operations to avoid data corruption
		regions_header_file_mark_operations_begin(file);

		// Enumerate map and write information
		node = saim_set_get_first(map->set);
		while (node != map->set->nil)
		{
			pair = (region_map_pair_t *)node->data;
			regions_header_file_write_region_stored_info(file, &pair->info);
			node = saim_set_get_next(map->set, node);
		}

		regions_header_file_mark_operations_end(file);
		// Finally
		saim_file_close(&file->file);

		return file->file.operation_successful;
	}
	else
		return false;
}
void regions_header_file_read_all_regions(regions_header_file_t * file, region_map_t * map)
{
	region_map_pair_t * pair;
	for (unsigned int i = 0; i < file->num_regions; ++i)
	{
		pair = (region_map_pair_t *)SAIM_MALLOC(sizeof(region_map_pair_t));
		region_map_pair_create(pair);
		regions_header_file_read_region_stored_info(file, &pair->info);
		saim_string_copy(&pair->name, &pair->info.info.name);
		// Insert a pair into map
		(void)region_map_insert(map, pair);
	}
}
bool regions_header_file_flush(regions_header_file_t * file)
{
	if (saim_file_open_for_write(&file->file))
	{
		file->unique_id = 0U;
		file->num_regions = 0U;
		file->file.operation_successful = true;
		regions_header_file_write_header(file);
		saim_file_close(&file->file);
		return file->file.operation_successful;
	}
	else
		return false;
}
void regions_header_file_regenerate(regions_header_file_t * file)
{
	saim_file_close(&file->file);
	if (saim_file_open_for_write(&file->file))
	{
		regions_header_file_write_header(file);
	}
	else
	{
		fprintf(stderr, "saim: file regeneration has failed.\n");
		file->file.operation_successful = false;
	}
}
void regions_header_file_mark_operations_begin(regions_header_file_t * file)
{
	file_offset_t position = saim_file_tell(&file->file);
	saim_file_offset_from_beginning(&file->file, kCorruptionByteOffset);
	saim_file_write_byte(&file->file, 1);
	saim_file_offset_from_beginning(&file->file, position);
}
void regions_header_file_mark_operations_end(regions_header_file_t * file)
{
	file_offset_t position = saim_file_tell(&file->file);
	saim_file_offset_from_beginning(&file->file, kCorruptionByteOffset);
	saim_file_write_byte(&file->file, 0);
	saim_file_offset_from_beginning(&file->file, position);
}
void regions_header_file_write_header(regions_header_file_t * file)
{
	saim_file_write_uint(&file->file, kFormatSignature);
	saim_file_write_byte(&file->file, kVersion);
	saim_file_write_byte(&file->file, 0); // corruption byte
	saim_file_write_uint(&file->file, file->unique_id);
	saim_file_write_uint(&file->file, file->num_regions);
}
void regions_header_file_write_region_info(regions_header_file_t * file, const region_info_t * region)
{
	if (0 == fwrite(&region->upper_latitude, sizeof(region->upper_latitude), 1, file->file.file))
	{
		fprintf(stderr, "saim: fwrite failed\n");
		file->file.operation_successful = false;
	}
	if (0 == fwrite(&region->left_longitude, sizeof(region->left_longitude), 1, file->file.file))
	{
		fprintf(stderr, "saim: fwrite failed\n");
		file->file.operation_successful = false;
	}
	if (0 == fwrite(&region->lower_latitude, sizeof(region->lower_latitude), 1, file->file.file))
	{
		fprintf(stderr, "saim: fwrite failed\n");
		file->file.operation_successful = false;
	}
	if (0 == fwrite(&region->right_longitude, sizeof(region->right_longitude), 1, file->file.file))
	{
		fprintf(stderr, "saim: fwrite failed\n");
		file->file.operation_successful = false;
	}
	saim_file_write_string(&file->file, &region->name);
}
void regions_header_file_write_region_stored_info(regions_header_file_t * file, const stored_region_info_t * region)
{
	regions_header_file_write_region_info(file, &region->info);
	saim_file_write_uint(&file->file, region->status);
	if (0 == fwrite(&region->time, sizeof(region->time), 1, file->file.file))
	{
		fprintf(stderr, "saim: fwrite failed\n");
		file->file.operation_successful = false;
	}
	saim_file_write_string(&file->file, &region->filename);
}
bool regions_header_file_read_header(regions_header_file_t * file)
{
	unsigned int signature;
	unsigned char version;
	unsigned char corrupted;
	// Read signature
	saim_file_read_uint(&file->file, &signature);
	// Check signature
	if (signature != kFormatSignature)
	{
		fprintf(stderr, "saim: wrong file signature, terminating\n");
		file->file.operation_successful = false;
		return false;
	}
	// Read version
	saim_file_read_byte(&file->file, &version);
	// Check version
	if (version < kVersion) // out of date
	{
		fprintf(stderr, "saim: format version %d is out of date. File will be cleaned.\n", version);
		regions_header_file_regenerate(file);
		return false;
	}
	// Read corruption byte
	saim_file_read_byte(&file->file, &corrupted);
	// Check corruption
	if (corrupted)
	{
		fprintf(stderr, "saim: file is broken. It will be cleaned.\n");
		regions_header_file_regenerate(file);
		return false;
	}
	// Read unique id
	saim_file_read_uint(&file->file, &file->unique_id);
	// Read number of regions
	saim_file_read_uint(&file->file, &file->num_regions);
	return true;
}
void regions_header_file_read_region_info(regions_header_file_t * file, region_info_t * region)
{
	if (0 == fread(&region->upper_latitude, sizeof(region->upper_latitude), 1, file->file.file))
	{
		fprintf(stderr, "saim: fread failed\n");
		file->file.operation_successful = false;
	}
	if (0 == fread(&region->left_longitude, sizeof(region->left_longitude), 1, file->file.file))
	{
		fprintf(stderr, "saim: fread failed\n");
		file->file.operation_successful = false;
	}
	if (0 == fread(&region->lower_latitude, sizeof(region->lower_latitude), 1, file->file.file))
	{
		fprintf(stderr, "saim: fread failed\n");
		file->file.operation_successful = false;
	}
	if (0 == fread(&region->right_longitude, sizeof(region->right_longitude), 1, file->file.file))
	{
		fprintf(stderr, "saim: fread failed\n");
		file->file.operation_successful = false;
	}
	saim_file_read_string(&file->file, &region->name);
}
void regions_header_file_read_region_stored_info(regions_header_file_t * file, stored_region_info_t * region)
{
	regions_header_file_read_region_info(file, &region->info);
	saim_file_read_uint(&file->file, &region->status);
	if (0 == fread(&region->time, sizeof(region->time), 1, file->file.file))
	{
		fprintf(stderr, "saim: fread failed\n");
		file->file.operation_successful = false;
	}
	saim_file_read_string(&file->file, &region->filename);
}