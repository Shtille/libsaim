/*************************************************************************
* libsaim 1.0
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

#ifndef __SAIM_TARGET_INFO_H__
#define __SAIM_TARGET_INFO_H__

#include <stdbool.h>

typedef struct saim_target_info saim_target_info;

/*! 
 * @brief A target info structure.
 */
struct saim_target_info {
	unsigned char* target_buffer;
	int target_width;
	int target_height;
	int target_bpp;
	void* bitmap_buffer; //!< pointer to saim_bitmap_buffer instance, for inner use
	void* line_buffer;   //!< pointer to saim_line_buffer instance, for inner use
};

/*!
 * Creates target information structure instance based on input parameters.
 * Don't forget to allocate memory for structure instance before passing it here.
 * 
 * @param[in,out] info    The info structure instance.
 * @param[in]     buffer  The pixel buffer to be filled during rendering.
 * @param[in]     width   The width of the pixel buffer.
 * @param[in]     height  The height of the pixel buffer.
 * @param[in]     bpp     The number of bytes per pixel of the pixel buffer.
 * 
 * @return True if success and false otherwise.
 */
bool saim_target_info__create(saim_target_info * info, unsigned char* buffer, int width, int height, int bpp);

/*!
 * Destroys previously created target information structure instance.
 */
void saim_target_info__destroy(saim_target_info * info);

#endif