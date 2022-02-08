/*************************************************************************
*  Test program for single threaded library usage. C version.
* ------------------------------------------------------------------------
*  Copyright (c) 2022 Vladimir Sviridov <v.shtille@gmail.com>
* 
*  This software is provided 'as-is', without any express or implied
*  warranty. In no event will the authors be held liable for any damages
*  arising from the use of this software.
* 
*  Permission is granted to anyone to use this software for any purpose,
*  including commercial applications, and to alter it and redistribute it
*  freely, subject to the following restrictions:
* 
*  1. The origin of this software must not be misrepresented; you must not
*     claim that you wrote the original software. If you use this software
*     in a product, an acknowledgment in the product documentation would
*     be appreciated but is not required.
* 
*  2. Altered source versions must be plainly marked as such, and must not
*     be misrepresented as being the original software.
* 
*  3. This notice may not be removed or altered from any source
*     distribution.
* 
**************************************************************************/

#include "../../../include/saim.h" // main library header

#include "../../../src/rasterizer/saim_decoder_jpeg.h" // to save final image

#include "../../../deps/tinycthread.h" // for thread routines

#include <stdlib.h>
#include <stdio.h>

int main()
{
    int result;
    saim_provider_info * info = 0;
    int flags = 0;
    unsigned char * buffer;
    int width, height, bytes_per_pixel;
    int num_tiles_left;
    double upper_latitude, left_longitude, lower_latitude, right_longitude;
	saim_bitmap bitmap;
    struct saim_instance * instance;

    // Initialize libsaim
    instance = saim_init("", info, flags, 1, &result);
    if (result != 0)
        return result;

    // Provide information about target surface
    width = 640;
    height = 480;
    bytes_per_pixel = 3;
    buffer = (unsigned char *) malloc(width * height * bytes_per_pixel);
    saim_set_target(instance, buffer, width, height, bytes_per_pixel);

    // Now we are able to render
    upper_latitude = 34.890285;
    lower_latitude = 31.816744;
    left_longitude = -118.596199;
    right_longitude = -114.489501;

    do
    {
        num_tiles_left = saim_render_aligned(instance, upper_latitude, left_longitude, lower_latitude, right_longitude);
        thrd_yield();
    }
    while (num_tiles_left > 0);

	// Store filled buffer to JPEG image
	bitmap.data = buffer;
	if (!saim_decoder_jpeg__save("out.jpg", 80, false, width, height, bytes_per_pixel, &bitmap))
		printf("image saving failed\n");

    // Don't forget to free buffer that we've been using
    free(buffer);

    // Cleanup libsaim
    saim_cleanup(instance);

    return 0;
}