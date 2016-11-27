#include "saim_file.h"

#include "util/saim_memory.h"

#include <string.h>

saim_file * saim_file_alloc(const char* filename)
{
	saim_file * file;
	file = (saim_file *)SAIM_MALLOC(sizeof(saim_file));
	saim_file_create(file, filename);
	return file;
}
void saim_file_free(saim_file * file)
{
	saim_file_destroy(file);
	SAIM_FREE(file);
}
void saim_file_create(saim_file * file, const char* filename)
{
	file->file = 0;
	file->filename = SAIM_STRDUP(filename);
	file->operation_successful = false;
	file->was_open_for_write = false;
}
void saim_file_destroy(saim_file * file)
{
	SAIM_FREE(file->filename);
	if (file->file)
	{
		fclose(file->file);
		file->file = 0;
	}
}
bool saim_file_open_for_write(saim_file * file)
{
	if (file->file) // opened
    {
        // Our main mode is read, so we gonna close the file
        fclose(file->file);
        file->was_open_for_write = false;
    }
    if ((file->file = fopen(file->filename, "wb")))
    {
        file->was_open_for_write = true;
        file->operation_successful = true;
        return true;
    }
    else
    {
        fprintf(stderr, "saim: fopen failed: %s", file->filename);
        return false;
    }
}
bool saim_file_open_for_read(saim_file * file)
{
	if (!file->file)
	{
	    if ((file->file = fopen(file->filename, "rb+"))) // we will also use this option to update the file
	    {
	        file->operation_successful = true;
	        return true;
	    }
	    else
	    {
	        fprintf(stderr, "saim: fopen failed: %s", file->filename);
	        return false;
	    }
	}
	else
	{
	    // File is already open for reading
	    saim_file_rewind(file);
	    return true;
	}
}
void saim_file_rewind(saim_file * file)
{
	rewind(file->file);
}
void saim_file_close(saim_file * file)
{
	if (file->file)
	{
		if (file->was_open_for_write)
		{
			fclose(file->file);
			file->file = NULL;
		}
	}
	file->was_open_for_write = false;
}
void saim_file_offset_from_beginning(saim_file * file, long offset)
{
	if (0 != fseek(file->file, offset, SEEK_SET))
	{
		fprintf(stderr, "saim: fseek failed");
		file->operation_successful = false;
	}
}
void saim_file_offset_from_current(saim_file * file, long offset)
{
	if (0 != fseek(file->file, offset, SEEK_CUR))
	{
		fprintf(stderr, "saim: fseek failed");
		file->operation_successful = false;
	}
}
void saim_file_offset_from_end(saim_file * file, long offset)
{
	if (0 != fseek(file->file, offset, SEEK_END))
	{
		fprintf(stderr, "saim: fseek failed");
		file->operation_successful = false;
	}
}
file_offset_t saim_file_tell(saim_file * file)
{
	return (file_offset_t)ftell(file->file);
}
void saim_file_write_byte(saim_file * file, unsigned char x)
{
	if (0 == fwrite(&x, sizeof(x), 1, file->file))
	{
		fprintf(stderr, "saim: fwrite failed");
		file->operation_successful = false;
	}
}
void saim_file_write_int(saim_file * file, int x)
{
	if (0 == fwrite(&x, sizeof(x), 1, file->file))
	{
		fprintf(stderr, "saim: fwrite failed");
		file->operation_successful = false;
	}
}
void saim_file_write_uint(saim_file * file, unsigned int x)
{
	if (0 == fwrite(&x, sizeof(x), 1, file->file))
	{
		fprintf(stderr, "saim: fwrite failed");
		file->operation_successful = false;
	}
}
void saim_file_write_array(saim_file * file, const void *data, unsigned int size)
{
	if (0 == fwrite(data, size, 1, file->file))
	{
		fprintf(stderr, "saim: fwrite failed");
		file->operation_successful = false;
	}
}
void saim_file_write_string(saim_file * file, const saim_string* str)
{
	if (0 == fwrite(&str->length, sizeof(unsigned int), 1, file->file))
	{
		fprintf(stderr, "saim: fwrite failed");
		file->operation_successful = false;
	}
	if (0 == fwrite(str->data, str->length * sizeof(char), 1, file->file))
	{
		fprintf(stderr, "saim: fwrite failed");
		file->operation_successful = false;
	}
}
void saim_file_read_byte(saim_file * file, unsigned char *x)
{
	if (0 == fread(x, sizeof(unsigned char), 1, file->file))
	{
		fprintf(stderr, "saim: fread failed");
		file->operation_successful = false;
	}
}
void saim_file_read_int(saim_file * file, int *x)
{
	if (0 == fread(x, sizeof(int), 1, file->file))
	{
		fprintf(stderr, "saim: fread failed");
		file->operation_successful = false;
	}
}
void saim_file_read_uint(saim_file * file, unsigned int *x)
{
	if (0 == fread(x, sizeof(unsigned int), 1, file->file))
	{
		fprintf(stderr, "saim: fread failed");
		file->operation_successful = false;
	}
}
void saim_file_read_array(saim_file * file, void *data, unsigned int size)
{
	if (0 == fread(data, size, 1, file->file))
	{
		fprintf(stderr, "saim: fread failed");
		file->operation_successful = false;
	}
}
void saim_file_read_string(saim_file * file, saim_string *str)
{
	unsigned int length;
	if (0 == fread(&length, sizeof(unsigned int), 1, file->file))
	{
		fprintf(stderr, "saim: fread failed");
		file->operation_successful = false;
	}
	saim_string_resize(str, length);
	if (0 == fread(str->data, str->length * sizeof(char), 1, file->file))
	{
		fprintf(stderr, "saim: fread failed");
		file->operation_successful = false;
	}
}