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


namespace pictura_mediocritas {
	/// Errors retutned from `pictura_mediocritas::output_image`.
	enum class output_image_result_t : std::uint8_t {
		/// No errors.
		ok,
		/// No image format could be deduced for the specified extension.
		deduction_error,
		/// Failed to save the image.
		save_error,
		/// Failed to set a colour.
		colour_set_error,
	};


	/// Write the specified image of a specified size and depth to the specified file.
	///
	/// When `format` is not provided -- deduce from extension.
	///
	/// When size in a pair, it's in the format `{width, height}`.
	template <class FrameT>
	output_image_result_t output_image(std::size_t width, std::size_t height, std::size_t channels, const FrameT & frame, const char * to);
	template <class FrameT>
	output_image_result_t output_image(std::size_t width, std::size_t height, std::size_t channels, FrameT & frame, const char * to);
	template <class FrameT>
	output_image_result_t output_image(std::size_t width, std::size_t height, std::size_t channels, const FrameT & frame, const char * to,
	                                   FREE_IMAGE_FORMAT format);
	template <class FrameT>
	output_image_result_t output_image(std::size_t width, std::size_t height, std::size_t channels, FrameT & frame, const char * to, FREE_IMAGE_FORMAT format);
	template <class FrameT>
	output_image_result_t output_image(std::pair<std::size_t, std::size_t> size, std::size_t channels, const FrameT & frame, const char * to);
	template <class FrameT>
	output_image_result_t output_image(std::pair<std::size_t, std::size_t> size, std::size_t channels, FrameT & frame, const char * to);
	template <class FrameT>
	output_image_result_t output_image(std::pair<std::size_t, std::size_t> size, std::size_t channels, const FrameT & frame, const char * to,
	                                   FREE_IMAGE_FORMAT format);
	template <class FrameT>
	output_image_result_t output_image(std::pair<std::size_t, std::size_t> size, std::size_t channels, FrameT & frame, const char * to, FREE_IMAGE_FORMAT format);
}


#include "output_image.inc"
