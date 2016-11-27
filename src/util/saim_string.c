#include "saim_string.h"

#include "saim_memory.h"

#include <string.h>
#include <assert.h>

unsigned int saim_hash(const char* buffer)
{
	/*
	Hash function suggested by http://www.cs.yorku.ca/~oz/hash.html
	Due to Dan Bernstein.  Allegedly very good on strings.
	*/
	unsigned int hash = 5381;
	int c;

	while ((c = *buffer++))
		hash = ((hash << 5) + hash) ^ c; /* hash * 33 ^ c */
	return hash;
}
unsigned int saim_string_hash(const saim_string * str)
{
	unsigned int hash = 5381, i = 0;
	const char* buffer = str->data;
	int c;

	while ((i++ < str->length) && ((c = *buffer++)))
		hash = ((hash << 5) + hash) ^ c;
	return hash;
}
void saim_string_create(saim_string * str)
{
	str->length = 0;
	str->data = 0;
}
void saim_string_destroy(saim_string * str)
{
	if (str->data)
	{
		SAIM_FREE(str->data);
		str->data = NULL;
	}
}
void saim_string_set_text(saim_string * str, const char* text)
{
	unsigned int length;
	length = (unsigned int)strlen(text);
	saim_string_resize(str, length);
	strncpy(str->data, text, length);
}
void saim_string_set(saim_string * str, const saim_string * other)
{
	saim_string_resize(str, other->length);
	strncpy(str->data, other->data, other->length);
}
void saim_string_resize(saim_string * str, unsigned int new_length)
{
	// Add rounding to power of vector-like formula
	if (str->length < new_length)
	{
		str->length = new_length;
		if (!str->data)
		{
			str->data = (char*)SAIM_MALLOC(new_length * sizeof(char));
		}
		else
		{
			str->data = (char*)SAIM_REALLOC(str->data, new_length * sizeof(char));
		}
	}
}
void saim_string_append(saim_string * str, const char* buffer, unsigned int length)
{
	unsigned int old_length = str->length;
	saim_string_resize(str, str->length + length);
	memcpy(str->data + old_length, buffer, length);
}
int saim_string_compare(const saim_string * str1, const saim_string * str2)
{
	if (str1->length < str2->length)
		return -1;
	else if (str1->length > str2->length)
		return 1;
	else // lengths are equal
		return strncmp(str1->data, str2->data, str1->length);
}
void saim_string_copy(saim_string * dest, const saim_string * src)
{
	assert(dest->data == NULL);
	dest->length = src->length;
	dest->data = (char*)SAIM_MALLOC(src->length * sizeof(char));
	strncpy(dest->data, src->data, src->length);
}