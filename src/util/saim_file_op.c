#include "saim_file_op.h"

#include <stdio.h>

void saim_file_op__delete(const char* filename)
{
	remove(filename);
}