#ifndef __SAIM_STORAGE_FILE_H__
#define __SAIM_STORAGE_FILE_H__

#include "saim_file.h"
#include "saim_key_offset_map.h"
#include "saim_map_nodes_list.h"

#define SAIM_FILE_BLOCK_CAPACITY 100

typedef struct {
	saim_file file;
	file_offset_t max_file_size; // up to 2 GB
	unsigned int hash_value;
	unsigned int block_capacity;
	unsigned int blocks_count;
	bool need_to_shrink;
} saim_storage_file;

typedef enum {
    kSave_Success,
    kSave_Failed,
    kSave_NeedToFlush
} save_result_t;

void saim_storage_file__create(saim_storage_file * file,
	const char* filename, unsigned int hash_value, file_offset_t max_file_size);
void saim_storage_file__destroy(saim_storage_file * file);

big_file_size_t saim_storage_file__get_estimated_file_size(unsigned int num_tiles, unsigned int data_size);

bool saim_storage_file__flush(saim_storage_file * file);

void saim_storage_file__regenerate(saim_storage_file * file);

void saim_storage_file__mark_operations_begin(saim_storage_file * file);
void saim_storage_file__mark_operations_end(saim_storage_file * file);

// Write operations
void saim_storage_file__write_header(saim_storage_file * file);

// Read operations
bool saim_storage_file__read_header(saim_storage_file * file);
void saim_storage_file__read_all_keys(saim_storage_file * file, saim_key_offset_map * offsets, saim_map_nodes_list * list);

save_result_t saim_storage_file__save_logics(saim_storage_file * file,
	const saim_data_key * key, const saim_string * data,
	saim_key_offset_map * offsets, saim_map_nodes_list * list);

typedef struct {
    unsigned int count;         //!< number of used blocks
    file_offset_t next_offset;	//!< offset to the next block
} block_header_t;

typedef struct {
    stored_key_pair_t key_offsets[SAIM_FILE_BLOCK_CAPACITY];
} block_keys_t;

// Some base operations
void saim_storage_file__write_key_pair(saim_storage_file * file, const stored_key_pair_t* pair);
void saim_storage_file__read_key_pair(saim_storage_file * file, stored_key_pair_t *pair);
void saim_storage_file__write_block_header(saim_storage_file * file, const block_header_t* block_header);
void saim_storage_file__read_block_header(saim_storage_file * file, block_header_t *block_header);
void saim_storage_file__write_block_keys(saim_storage_file * file, const block_keys_t* keys);
void saim_storage_file__read_block_keys(saim_storage_file * file, block_keys_t *keys);
save_result_t saim_storage_file__replace_key(saim_storage_file * file,
	const saim_data_key* key, const saim_string * data,
    saim_key_offset_map * offsets, saim_map_nodes_list * list);

#endif