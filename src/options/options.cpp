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


#include "options.hpp"
#include "../util.hpp"
#include "existing_file_constraint.hpp"
#include "existing_parent_dir_constraint.hpp"
#include <tclap/CmdLine.h>
#include <tclap/SwitchArg.h>
#include <tclap/ValueArg.h>


using namespace std::literals;


std::tuple<pictura_mediocritas::options, int, std::string> pictura_mediocritas::options::parse(int argc, const char * const * argv) {
	options ret;

	try {
		pictura_mediocritas::existing_file_constraint input_video_constraint("input video");
		pictura_mediocritas::existing_parent_dir_constraint out_image_constraint("output image");

		TCLAP::CmdLine command_line("Pictūra Mediocritas -- like aurea mediocritas, but with frames in a video instead", ' ', PICTURA_MEDIOCRITAS_VERSION);
		TCLAP::UnlabeledValueArg<std::string> in_video("in_video", "Video to average", true, "", &input_video_constraint, command_line);
		TCLAP::UnlabeledValueArg<std::string> out_image("out_image", "Image to write the average frame to. Default: in_video.png", false, "", &out_image_constraint,
		                                                command_line);

		command_line.setExceptionHandling(false);
		command_line.parse(argc, argv);

		ret.in_video = in_video;

		if(out_image.getValue().empty())
			ret.out_image = switch_extenstion(ret.in_video, "png");
		else
			ret.out_image = out_image;
	} catch(const TCLAP::ArgException & e) {
		auto arg_id = e.argId();
		if(arg_id == " ")
			arg_id = "undefined argument";
		return std::make_tuple(ret, 1, std::string(argv[0]) + ": error: parsing arguments failed (" + e.error() + ") for " + arg_id);
	} catch(const TCLAP::ExitException & e) {
		return std::make_tuple(ret, e.getExitStatus() ? e.getExitStatus() : 1, "");
	}

	return std::make_tuple(ret, 0, ""s);
}

bool pictura_mediocritas::operator==(const options & lhs, const options & rhs) {
	return lhs.in_video == rhs.in_video && lhs.out_image == rhs.out_image;
}

bool pictura_mediocritas::operator!=(const options & lhs, const options & rhs) {
	return !(lhs == rhs);
}
