#ifndef __SAIM_H__
#define __SAIM_H__

#include "saim_provider_info.h"

int saim_init(const char* path, saim_provider_info * provider_info, int flags);
void saim_cleanup();

void saim_set_target(unsigned char * buffer, int width, int height, int bytes_per_pixel);
/*! Renders saved bitmap onto target surface
@param[in] upper_latitude Latitude of the upper left point of rendered rectangle
TODO: discribe later
*/
int saim_render_aligned(double upper_latitude, double left_longitude, double lower_latitude, double right_longitude);
int saim_render_common(double upper_latitude, double left_longitude, double lower_latitude, double right_longitude, float angle);

#endif