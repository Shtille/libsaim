#ifndef __SAIM_BITMAP_H__
#define __SAIM_BITMAP_H__

typedef struct {
	unsigned char* data;
} saim_bitmap;

void saim_bitmap__create(saim_bitmap * bitmap);
void saim_bitmap__destroy(saim_bitmap * bitmap);

#endif