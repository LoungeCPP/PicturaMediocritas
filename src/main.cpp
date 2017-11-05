// The MIT License (MIT)

// Copyright (c) 2017 Lounge<C++>

// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


#include "average_frame.hpp"
#include "options/options.hpp"
#include "util.hpp"
#include <FreeImage.h>


int main(int argc, const char ** argv) {
	const auto opts_r = pictura_mediocritas::options::parse(argc, argv);
	if(std::get<1>(opts_r)) {
		std::cerr << std::get<2>(opts_r) << '\n';
		return std::get<1>(opts_r);
	}
	const auto opts = std::move(std::get<0>(opts_r));

	FreeImage_Initialise();
	pictura_mediocritas::quickscope_wrapper freeimage_deinitialiser{FreeImage_DeInitialise};

	std::cout << "Averaging " << opts.in_video << " into " << opts.out_image << ".\n";

	pictura_mediocritas::average_frame_u64 frame(0, 0);

	if(pictura_mediocritas::has_extension(opts.in_video.c_str(), "gif")) {
		const auto in_video = FreeImage_OpenMultiBitmap(FIF_GIF, opts.in_video.c_str(), false, true, true, GIF_LOAD256 | GIF_PLAYBACK);
		pictura_mediocritas::quickscope_wrapper in_video_close{[&]() { FreeImage_CloseMultiBitmap(in_video); }};

		std::vector<std::uint8_t> bytes;

		const std::size_t pages = FreeImage_GetPageCount(in_video);
		for(std::size_t i = 0; i < pages; ++i) {
			const auto page = FreeImage_LockPage(in_video, i);
			pictura_mediocritas::quickscope_wrapper page_unlock{[&]() { FreeImage_UnlockPage(in_video, page, false); }};

			if(i == 0) {
				const std::size_t w = FreeImage_GetWidth(page);
				const std::size_t h = FreeImage_GetHeight(page);

				frame = pictura_mediocritas::average_frame_u64(w, h);
				bytes.resize(w * h * decltype(frame)::channels);
			}

			const auto size   = frame.size();
			const auto width  = size.first;
			const auto height = size.second;
			for(auto x = 0u; x < width; ++x)
				for(auto y = 0u; y < height; ++y) {
					RGBQUAD colour{};
					if(FreeImage_GetPixelColor(page, x, y, &colour)) {
						bytes[(y * width + x) * decltype(frame)::channels]     = colour.rgbRed;
						bytes[(y * width + x) * decltype(frame)::channels + 1] = colour.rgbGreen;
						bytes[(y * width + x) * decltype(frame)::channels + 2] = colour.rgbBlue;
						bytes[(y * width + x) * decltype(frame)::channels + 3] = colour.rgbReserved;
						std::cerr << static_cast<unsigned>(colour.rgbReserved) << '\n';
					} else {
						std::cerr << "Failed to get pixel " << x << 'x' << y << " frame #" << i << ".\n";
						return 1;
					}
				}

			frame.process_frame(bytes);
		}
	} else {
		std::cerr << "Could not find codec for " << opts.in_video << ".\n";
		return 1;
	}

	auto out_image = FreeImage_AllocateT(FIT_BITMAP, frame.size().first, frame.size().second, 32);
	pictura_mediocritas::quickscope_wrapper out_image_unloader{[&]() { FreeImage_Unload(out_image); }};

	RGBQUAD px{};
	for(auto x = 0u; x < frame.size().first; ++x)
		for(auto y = 0u; y < frame.size().second; ++y) {
			px.rgbRed      = frame[(y * frame.size().first + x) * decltype(frame)::channels];
			px.rgbGreen    = frame[(y * frame.size().first + x) * decltype(frame)::channels + 1];
			px.rgbBlue     = frame[(y * frame.size().first + x) * decltype(frame)::channels + 2];
			px.rgbReserved = frame[(y * frame.size().first + x) * decltype(frame)::channels + 3];
			if(!FreeImage_SetPixelColor(out_image, x, y, &px))
				std::cerr << "Failed to set pixel " << x << 'x' << y << ".\n";
		}

	const auto format = pictura_mediocritas::deduce_image_format(opts.out_image.c_str());
	if(format != FIF_UNKNOWN) {
		if(!FreeImage_Save(format, out_image, opts.out_image.c_str())) {
			std::cerr << "Failed to write " << opts.out_image << ".\n";
			return 1;
		}
	} else {
		std::cerr << "Could not find write codec for " << opts.out_image << ".\n";
		return 1;
	}
}
