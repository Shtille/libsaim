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

#ifndef __SAIM_LINE_BUFFER_H__
#define __SAIM_LINE_BUFFER_H__

typedef struct saim_line_buffer saim_line_buffer;

/*! 
 * @brief A line buffer structure for aligned and common rendering.
 */
struct saim_line_buffer {
	int x_buffer_size;      //!< size of x buffer
	int y_buffer_size;      //!< size of y buffer
	int * x_keys;           //!< transforms screen pixel x to key x
	int * x_samples;        //!< transforms screen pixel x to sample x
	int * x_pixels;         //!< transforms screen pixel x to bitmap pixel x
	int * y_keys;           //!< transforms screen pixel y to key y
	int * y_samples;        //!< transforms screen pixel y to sample y
	int * y_pixels;         //!< transforms screen pixel y to bitmap pixel y
};

#endif