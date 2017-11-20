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


#include "output_image.hpp"
#include "util.hpp"
#include <iostream>


#define FROM_BODY()                                                            \
	do {                                                                         \
		RGBQUAD px{};                                                              \
		for(auto y = 0u; y < height; ++y) {                                        \
			const auto column = y * width;                                           \
			for(auto x = 0u; x < width; ++x) {                                       \
				const auto start = (column + x) * channels;                            \
				switch(channels) {                                                     \
					case 4:                                                              \
						px.rgbReserved = frame[start + 3];                                 \
						[[fallthrough]];                                                   \
					case 3:                                                              \
						px.rgbBlue = frame[start + 2];                                     \
						[[fallthrough]];                                                   \
					case 2:                                                              \
						px.rgbGreen = frame[start + 1];                                    \
						[[fallthrough]];                                                   \
					case 1:                                                              \
						px.rgbRed = frame[start];                                          \
				}                                                                      \
                                                                               \
				if(!FreeImage_SetPixelColor(out_image, x, y, &px)) {                   \
					std::cerr << "Failed to set pixel " << x << 'x' << y << ".\n";       \
					return pictura_mediocritas::output_image_result_t::colour_set_error; \
				}                                                                      \
			}                                                                        \
		}                                                                          \
	} while(false)

#define OUTPUT_IMAGE_DEDUCE_BODY()                                        \
	do {                                                                    \
		const auto format = pictura_mediocritas::deduce_image_format(to);     \
		if(format == FIF_UNKNOWN)                                             \
			return pictura_mediocritas::output_image_result_t::deduction_error; \
		else                                                                  \
			return output_image(width, height, channels, frame, to, format);    \
	} while(false)

#define OUTPUT_IMAGE_SET_FORMAT_BODY()                                                                  \
	do {                                                                                                  \
		auto out_image = FreeImage_AllocateT(FIT_BITMAP, width, height, channels * 8);                      \
		pictura_mediocritas::quickscope_wrapper out_image_unloader{[&]() { FreeImage_Unload(out_image); }}; \
                                                                                                        \
		FROM_BODY();                                                                                        \
                                                                                                        \
		if(!FreeImage_Save(format, out_image, to))                                                          \
			return pictura_mediocritas::output_image_result_t::save_error;                                    \
		else                                                                                                \
			return pictura_mediocritas::output_image_result_t::ok;                                            \
	} while(false)


template <class FrameT>
pictura_mediocritas::output_image_result_t pictura_mediocritas::output_image(std::size_t width, std::size_t height, std::size_t channels, const FrameT & frame,
                                                                             const char * to) {
	OUTPUT_IMAGE_DEDUCE_BODY();
}

template <class FrameT>
pictura_mediocritas::output_image_result_t pictura_mediocritas::output_image(std::size_t width, std::size_t height, std::size_t channels, FrameT & frame,
                                                                             const char * to) {
	OUTPUT_IMAGE_DEDUCE_BODY();
}

template <class FrameT>
pictura_mediocritas::output_image_result_t pictura_mediocritas::output_image(std::size_t width, std::size_t height, std::size_t channels, const FrameT & frame,
                                                                             const char * to, FREE_IMAGE_FORMAT format) {
	OUTPUT_IMAGE_SET_FORMAT_BODY();
}

template <class FrameT>
pictura_mediocritas::output_image_result_t pictura_mediocritas::output_image(std::size_t width, std::size_t height, std::size_t channels, FrameT & frame,
                                                                             const char * to, FREE_IMAGE_FORMAT format) {
	OUTPUT_IMAGE_SET_FORMAT_BODY();
}

template <class FrameT>
pictura_mediocritas::output_image_result_t pictura_mediocritas::output_image(std::pair<std::size_t, std::size_t> size, std::size_t channels,
                                                                             const FrameT & frame, const char * to) {
	return output_image(size.first, size.second, channels, frame, to);
}

template <class FrameT>
pictura_mediocritas::output_image_result_t pictura_mediocritas::output_image(std::pair<std::size_t, std::size_t> size, std::size_t channels,
                                                                             FrameT & frame, const char * to) {
	return output_image(size.first, size.second, channels, frame, to);
}

template <class FrameT>
pictura_mediocritas::output_image_result_t pictura_mediocritas::output_image(std::pair<std::size_t, std::size_t> size, std::size_t channels,
                                                                             const FrameT & frame, const char * to, FREE_IMAGE_FORMAT format) {
	return output_image(size.first, size.second, channels, frame, to, format);
}

template <class FrameT>
pictura_mediocritas::output_image_result_t pictura_mediocritas::output_image(std::pair<std::size_t, std::size_t> size, std::size_t channels, FrameT & frame,
                                                                             const char * to, FREE_IMAGE_FORMAT format) {
	return output_image(size.first, size.second, channels, frame, to, format);
}


#undef OUTPUT_IMAGE_SET_FORMAT_BODY
#undef OUTPUT_IMAGE_DEDUCE_BODY
#undef FROM_BODY
