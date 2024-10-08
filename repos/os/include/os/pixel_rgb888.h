/*
 * \brief  Template specializations for the RGB888 pixel format
 * \author Norman Feske
 * \date   2014-08-14
 */

/*
 * Copyright (C) 2014-2017 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#ifndef _INCLUDE__OS__PIXEL_RGB888_H_
#define _INCLUDE__OS__PIXEL_RGB888_H_

#include <os/pixel_rgba.h>

namespace Genode {

	using Pixel_rgb888 = Pixel_rgba<uint32_t, Surface_base::RGB888,
	                                0xff0000, 16, 0xff00, 8, 0xff, 0, 0, 0>;

	template <>
	inline Pixel_rgb888 Pixel_rgb888::avr(Pixel_rgb888 p1, Pixel_rgb888 p2)
	{
		Pixel_rgb888 res;
		res.pixel = (((p1.pixel&0xfe00fe00)>>1) + ((p2.pixel&0xfe00fe00)>>1))
		          | (((p1.pixel&0x00fe00fe)>>1) + ((p2.pixel&0x00fe00fe)>>1));
		return res;
	}

	template <>
	inline Pixel_rgb888 Pixel_rgb888::blend(Pixel_rgb888 src, int alpha)
	{
		Pixel_rgb888 res;
		res.pixel = (alpha * ((src.pixel & 0xff00) >> 8) & 0xff00)
		          | (((alpha * (src.pixel & 0xff00ff)) >> 8) & 0xff00ff);
		return res;
	}


	template <>
	inline Pixel_rgb888 Pixel_rgb888::mix(Pixel_rgb888 p1, Pixel_rgb888 p2, int alpha)
	{
		Pixel_rgb888 res;

		res.pixel = blend(p1, 256 - alpha).pixel + blend(p2, alpha).pixel;
		return res;
	}
}

#endif /* _INCLUDE__OS__PIXEL_RGB888_H_ */
