#ifndef __SAIM_MAP_H__
#define __SAIM_MAP_H__

typedef struct saim_map_node {
  void * key;
  void * value;
  int red; /* if red = 0 then the node is black */
  struct saim_map_node * left;
  struct saim_map_node * right;
  struct saim_map_node * parent;
} saim_map_node;


/* compare_func(a,b) should return 1 if *a > *b, -1 if *a < *b, and 0 otherwise */
/* destroy_func(a) takes a pointer to whatever data might be and frees it accordingly */
typedef struct {
	int  (*compare_func)(const void*, const void*);
	void (*key_destroy_func)(void*);
	void (*value_destroy_func)(void*);
	saim_map_node * root;
	saim_map_node * nil;
} saim_map;

saim_map * saim_map_create(int  (*compare_func)(const void*, const void*),
						   void (*key_destroy_func)(void*),
						   void (*value_destroy_func)(void*));
void saim_map_destroy(saim_map * map);

void saim_map_clear(saim_map * map);
saim_map_node * saim_map_insert(saim_map * map, void * key, void * value);
void saim_map_delete(saim_map * map, saim_map_node * node);

saim_map_node * saim_map_search(saim_map * map, void * key);

// Functions for tree enumeration:
saim_map_node * saim_map_get_first(saim_map * map);
saim_map_node * saim_map_get_next(saim_map * map, saim_map_node * prev);

#endif
