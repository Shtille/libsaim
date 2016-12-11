#include "saim_data_pair.h"

void saim_data_pair__create(saim_data_pair * data_pair)
{
	saim_string_create(&data_pair->data);
}
void saim_data_pair__destroy(saim_data_pair * data_pair)
{
	saim_string_destroy(&data_pair->data);
}