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


#define CATCH_CONFIG_ENABLE_TUPLE_STRINGMAKER
#include <catch.hpp>

#include "options/options.hpp"
#include "test_util.hpp"
#include <fstream>
#include <string>
#include <tuple>


using namespace std::literals;


namespace Catch {
	template <>
	struct StringMaker<pictura_mediocritas::options> {
		static std::string convert(const pictura_mediocritas::options & value) {
			return "{in_video=\'" + value.in_video + "\', out_image=\'" + value.out_image + "\'}";  //
		}
	};
}


TEST_CASE("pictura_mediocritas::options::parse() -- not enough args", "[options]") {
	reset_TCLAP();
	const char * args[] = {"pictura-mediocritas tests", nullptr};
	REQUIRE(pictura_mediocritas::options::parse(sizeof args / sizeof *args - 1, args) ==
	        std::make_tuple(pictura_mediocritas::options{}, 1,
	                        "pictura-mediocritas tests: error: parsing arguments failed (Required argument missing: in_video) for undefined argument"));
}

TEST_CASE("pictura_mediocritas::options::parse() -- nonexistant input file", "[options][incorrect]") {
	const auto temp = temp_dir() + "/PicturaMediocritas/options/incorrect/nonexistant-input/"s;
	make_directory_recursive(temp.c_str());

	const auto nonexistant_file = temp + "nonexistant_file";

	reset_TCLAP();
	const char * args[] = {"pictura-mediocritas tests", nonexistant_file.c_str(), nullptr};
	REQUIRE(pictura_mediocritas::options::parse(sizeof args / sizeof *args - 1, args) ==
	        std::make_tuple(pictura_mediocritas::options{}, 1,
	                        "pictura-mediocritas tests: error: parsing arguments failed (Value '" + nonexistant_file +
	                            "' does not meet constraint: existing file) for Argument: (--in_video)"));
}

TEST_CASE("pictura_mediocritas::options::parse() -- nonexistant output file parent", "[options][incorrect]") {
	const auto temp = temp_dir() + "/PicturaMediocritas/options/incorrect/nonexistant-input/"s;
	make_directory_recursive(temp.c_str());

	const auto in_video        = temp + "in_video.avi";
	const auto unparented_file = temp + "nonexistant_dir/unparented_file";
	std::ofstream{in_video};

	reset_TCLAP();
	const char * args[] = {"pictura-mediocritas tests", in_video.c_str(), unparented_file.c_str(), nullptr};
	REQUIRE(pictura_mediocritas::options::parse(sizeof args / sizeof *args - 1, args) ==
	        std::make_tuple(pictura_mediocritas::options{}, 1,
	                        "pictura-mediocritas tests: error: parsing arguments failed (Value '" + unparented_file +
	                            "' does not meet constraint: creatable file) for Argument: (--out_image)"));
}

TEST_CASE("pictura_mediocritas::options::parse() -- output file a dir", "[options][incorrect]") {
	const auto temp = temp_dir() + "/PicturaMediocritas/options/incorrect/nonexistant-input/"s;
	make_directory_recursive(temp.c_str());

	const auto in_video = temp + "in_video.avi";
	std::ofstream{in_video};

	reset_TCLAP();
	const char * args[] = {"pictura-mediocritas tests", in_video.c_str(), temp.c_str(), nullptr};
	REQUIRE(pictura_mediocritas::options::parse(sizeof args / sizeof *args - 1, args) ==
	        std::make_tuple(pictura_mediocritas::options{}, 1,
	                        "pictura-mediocritas tests: error: parsing arguments failed (Value '" + temp +
	                            "' does not meet constraint: creatable file) for Argument: (--out_image)"));
}

TEST_CASE("pictura_mediocritas::options::parse() -- correct", "[options]") {
	const auto temp = temp_dir() + "/PicturaMediocritas/options/correct/"s;
	make_directory_recursive(temp.c_str());

	const auto in_video     = temp + "in_video";
	const auto in_video_ext = temp + "in_video.mp4";
	const auto out_image    = temp + "out_image.jpeg2000";
	std::ofstream{in_video};
	std::ofstream{in_video_ext};

	{
		reset_TCLAP();
		const char * args[] = {"pictura-mediocritas tests", in_video.c_str(), nullptr};
		REQUIRE(pictura_mediocritas::options::parse(sizeof args / sizeof *args - 1, args) ==
		        std::make_tuple(pictura_mediocritas::options{in_video, in_video + ".png"}, 0, ""));
	}

	{
		reset_TCLAP();
		const char * args[] = {"pictura-mediocritas tests", in_video_ext.c_str(), nullptr};
		REQUIRE(pictura_mediocritas::options::parse(sizeof args / sizeof *args - 1, args) ==
		        std::make_tuple(pictura_mediocritas::options{in_video_ext, in_video + ".png"}, 0, ""));
	}

	{
		reset_TCLAP();
		const char * args[] = {"pictura-mediocritas tests", in_video.c_str(), out_image.c_str(), nullptr};
		REQUIRE(pictura_mediocritas::options::parse(sizeof args / sizeof *args - 1, args) ==
		        std::make_tuple(pictura_mediocritas::options{in_video, out_image}, 0, ""));
	}

	{
		reset_TCLAP();
		const char * args[] = {"pictura-mediocritas tests", in_video_ext.c_str(), out_image.c_str(), nullptr};
		REQUIRE(pictura_mediocritas::options::parse(sizeof args / sizeof *args - 1, args) ==
		        std::make_tuple(pictura_mediocritas::options{in_video_ext, out_image}, 0, ""));
	}
}
