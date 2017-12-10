// The MIT License (MIT)

// Copyright (c) 2017 Lounge<C++>

// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.


#include "multi_image.hpp"
#include <iostream>


void pictura_mediocritas::freeimage_multi_bitmap_deleter::operator()(FIMULTIBITMAP * multi_bitmap) const noexcept {
	FreeImage_CloseMultiBitmap(multi_bitmap);
}

void pictura_mediocritas::freeimage_page_unlocker::operator()(FIBITMAP * page) const noexcept {
	FreeImage_UnlockPage(owner, page, changed);
}


void pictura_mediocritas::multi_image_parser::lock_page() {
	cur_page.reset(FreeImage_LockPage(image.get(), cur_page_idx));
	width  = 0;
	height = 0;
}

pictura_mediocritas::multi_image_parser::multi_image_parser(FIMULTIBITMAP * i, std::size_t c)
      : channels(c), image(i), cur_page(0, freeimage_page_unlocker{i, false}), cur_page_idx(0), pages(0), width(0), height(0), cached(false) {
	lock_page();
}

std::pair<std::size_t, std::size_t> pictura_mediocritas::multi_image_parser::size() noexcept {
	if(cur_page && (!width || !height)) {
		width  = FreeImage_GetWidth(cur_page.get());
		height = FreeImage_GetHeight(cur_page.get());
	}

	return {width, height};
}

std::size_t pictura_mediocritas::multi_image_parser::length() noexcept {
	if(!pages)
		pages = FreeImage_GetPageCount(image.get());

	return pages;
}

void pictura_mediocritas::multi_image_parser::next() {
	++cur_page_idx;
	lock_page();
}

std::uint8_t pictura_mediocritas::multi_image_parser::operator[](std::size_t idx) {
	if(!channels)
		return -1;

	const auto width = size().first;
	if(!width)
		return -1;

	const auto channel = idx % channels;  // idx = (y * width + x) * decltype(frame)::channels + channel
	idx -= channel;                       // idx = (y * width + x) * decltype(frame)::channels
	idx /= channels;                      // idx =  y * width + x
	const auto x = idx % width;           //
	idx -= x;                             // idx =  y * width
	idx /= width;                         // idx =  y
	const auto y = idx;                   //

	if(!cached || (cache_x != x || cache_y != y)) {
		cached  = FreeImage_GetPixelColor(cur_page.get(), x, y, &cache);
		cache_x = x;
		cache_y = y;
	}

	if(cached)
		switch(channel) {
			case 0:
				return cache.rgbRed;
			case 1:
				return cache.rgbGreen;
			case 2:
				return cache.rgbBlue;
			case 3:
				return cache.rgbReserved;
		}

	std::cerr << "Failed to get pixel " << x << 'x' << y << " frame #" << cur_page_idx << ".\n";
	return -1;
}
