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


#pragma once


#include <FreeImage.h>
#include <cstdint>
#include <memory>


namespace pictura_mediocritas {
	struct freeimage_multi_bitmap_deleter {
		void operator()(FIMULTIBITMAP * multi_bitmap) const noexcept;
	};

	struct freeimage_page_unlocker {
		FIMULTIBITMAP * owner;
		bool changed;

		void operator()(FIBITMAP * page) const noexcept;
	};

	class multi_image_parser {
	private:
		std::size_t channels;

		std::unique_ptr<FIMULTIBITMAP, freeimage_multi_bitmap_deleter> image;
		std::unique_ptr<FIBITMAP, freeimage_page_unlocker> cur_page;
		std::size_t cur_page_idx;

		std::size_t pages;
		std::size_t width;
		std::size_t height;

		bool cached;
		RGBQUAD cache;
		std::size_t cache_x;
		std::size_t cache_y;

		void lock_page();

	public:
		/// Create a parser from an opened multi-image and the channel count in the target frame.
		multi_image_parser(FIMULTIBITMAP * image, std::size_t channels);


		/// Get current frame's size as `{width, height}`.
		std::pair<std::size_t, std::size_t> size() noexcept;

		/// Get frame count.
		std::size_t length() noexcept;

		/// Load the next frame.
		void next();

		/// Get the specified colour fragment.
		///
		/// Valid inside [0; width * height * channels) ∩ ℤ, undefined behaviour thereoutside.
		///
		/// `idx` is in the format `(y * width + x) * decltype(frame)::channels + channel`
		/// (i.e. the one required by `average_frame`).
		std::uint8_t operator[](std::size_t idx);
	};
}
