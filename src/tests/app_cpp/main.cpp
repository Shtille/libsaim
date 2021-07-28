#include "../../../include/saim.h" // main library header

#include <cstdlib>
#include <cstdio>
#include <thread>

int main()
{
    int result;
    saim_provider_info * info = 0;
    int flags = 0;
    unsigned char * buffer;
    int width, height, bytes_per_pixel;
    int num_tiles_left;
    double upper_latitude, left_longitude, lower_latitude, right_longitude;

    // Initialize libsaim
    result = saim_init("", info, flags, 1);

    // Provide information about target surface
    width = 640;
    height = 480;
    bytes_per_pixel = 3;
    buffer = (unsigned char *) malloc(width * height * bytes_per_pixel);
    saim_set_target(buffer, width, height, bytes_per_pixel);

    // Now we are able to render
    upper_latitude = 34.890285;
    lower_latitude = 31.816744;
    left_longitude = -118.596199;
    right_longitude = -114.489501;

    do
    {
        num_tiles_left = saim_render_aligned(upper_latitude, left_longitude, lower_latitude, right_longitude);
        std::this_thread::yield();
    }
    while (num_tiles_left > 0);

    printf("target image has been successfully rasterized\n");

    // Don't forget to free buffer that we've been using
    free(buffer);

    // Cleanup libsaim
    saim_cleanup();

    return 0;
}