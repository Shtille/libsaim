#include "saim.h"

#include <string.h>

static char s_path[260];

void saim_init(const char* path)
{
	strcpy(s_path, path);
}