#ifndef __SAIM_H__
#define __SAIM_H__

#include "saim_provider_info.h"

int saim_init(const char* path, saim_provider_info * provider_info);
void saim_cleanup();

#endif