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


#include "progressbar/progressbar.hpp"

#include "average_frame.hpp"
#include "options/options.hpp"
#include "output_image.hpp"
#include "parser/ffmpeg.hpp"
#include "parser/multi_image.hpp"
#include "util.hpp"
#include <FreeImage.h>
#include <cassert>
#include <iostream>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}


int main(int argc, const char ** argv) {
	const auto opts_r = pictura_mediocritas::options::parse(argc, argv);
	if(std::get<1>(opts_r)) {
		std::cerr << std::get<2>(opts_r) << '\n';
		return std::get<1>(opts_r);
	}
	const auto opts = std::move(std::get<0>(opts_r));

#ifdef _WIN32
	CoInitialize(nullptr);
#endif

	FreeImage_Initialise();
	pictura_mediocritas::quickscope_wrapper freeimage_deinitialiser{FreeImage_DeInitialise};

	pictura_mediocritas::average_frame_u64 avg_frame(0, 0);

	if(pictura_mediocritas::has_extension(opts.in_video.c_str(), "gif")) {
		pictura_mediocritas::multi_image_parser parser(FreeImage_OpenMultiBitmap(FIF_GIF, opts.in_video.c_str(), false, true, true, GIF_LOAD256 | GIF_PLAYBACK),
		                                               decltype(avg_frame)::channels);
		pictura_mediocritas::progressbar progress("Processing " + opts.in_video + ' ', parser.length());
		avg_frame = decltype(avg_frame)(parser.size());

		for(auto i = 0u; i < parser.length(); ++i) {
			avg_frame.process_frame(parser);
			parser.next();
			progress.inc();
		}

		progress.finish();
	} else {
		pictura_mediocritas::ffmpeg_parser parser(opts.in_video.c_str(), decltype(avg_frame)::channels);
		if(parser) {
			std::unique_ptr<pictura_mediocritas::progressbar> progress;
			if(!parser.process([&]() {
				   if(!progress)
					   progress = std::make_unique<pictura_mediocritas::progressbar>("Processing " + opts.in_video + ' ', parser.length());
				   if(avg_frame.size().first == 0)
					   avg_frame = decltype(avg_frame)(parser.size());

				   avg_frame.process_frame(parser);
				   progress->inc();

				   return true;
			   })) {
				std::cerr << "\nParsing " << opts.in_video << " failed: " << *parser.error() << '\n';
				return 1;
			} else
				progress->finish();
		} else if(parser.error() == "") {
			std::cerr << "Couldn't open " << opts.in_video << ".\n";
			return 1;
		} else {
			std::cerr << "Could not find codec for " << opts.in_video << ": " << *parser.error() << '\n';
			return 1;
		}
	}


	std::cout << "\nWriting to " << opts.out_image << '\n';
	switch(pictura_mediocritas::output_image(avg_frame.size(), decltype(avg_frame)::channels, avg_frame, opts.out_image.c_str())) {
		case pictura_mediocritas::output_image_result_t::ok:
			break;
		case pictura_mediocritas::output_image_result_t::deduction_error:
			std::cerr << "Could not find write codec for " << opts.out_image << ".\n";
			return 1;
		case pictura_mediocritas::output_image_result_t::save_error:
			std::cerr << "Failed to write " << opts.out_image << ".\n";
			return 1;
		case pictura_mediocritas::output_image_result_t::colour_set_error:
			return 1;
	}
}
