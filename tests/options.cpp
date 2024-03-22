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


#include <doctest/doctest.h>

#include "options.hpp"
#include "test_util.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <tuple>

using namespace std::literals;


namespace doctest {
	template <>
	struct StringMaker<pictura_mediocritas::options> {
		static String convert(const pictura_mediocritas::options & value) {
			return (((("{in_video=\'"s += value.in_video) += "\', out_image=\'") += value.out_image) + "\'}").c_str();
		}
	};
}

#define TUPLEQ(l_, r_)                         \
	{                                            \
		auto l = l_;                               \
		auto r = r_;                               \
		REQUIRE(std::get<0>(l) == std::get<0>(r)); \
		REQUIRE(std::get<1>(l) == std::get<1>(r)); \
		REQUIRE(std::get<2>(l) == std::get<2>(r)); \
	}


TEST_CASE("pictura_mediocritas::options::parse() -- not enough args") {
	const char * args[] = {"pictura-mediocritas-tests", nullptr};
	TUPLEQ(pictura_mediocritas::options::parse(sizeof args / sizeof *args - 1, args),
	       std::make_tuple(pictura_mediocritas::options{}, 1, "usage: pictura-mediocritas-tests in-video [out-image]"));
}

TEST_CASE("pictura_mediocritas::options::parse() -- too many args") {
	const char * args[] = {"pictura-mediocritas-tests", "in", "out", "extra", nullptr};
	TUPLEQ(pictura_mediocritas::options::parse(sizeof args / sizeof *args - 1, args),
	       std::make_tuple(pictura_mediocritas::options{"in", "out"}, 1, "usage: pictura-mediocritas-tests in-video [out-image]"));
}

TEST_CASE("pictura_mediocritas::options::parse() -- correct") {
	const auto temp = temp_dir() + "/PicturaMediocritas/options/correct/"s;
	make_directory_recursive(temp.c_str());

	const auto in_video     = temp + "in_video";
	const auto in_video_ext = temp + "in_video.mp4";
	const auto out_image    = temp + "out_image.jpeg2000";
	std::ofstream{in_video};
	std::ofstream{in_video_ext};

	{
		const char * args[] = {"pictura-mediocritas-tests", in_video.c_str(), nullptr};
		TUPLEQ(pictura_mediocritas::options::parse(sizeof args / sizeof *args - 1, args),
		       std::make_tuple(pictura_mediocritas::options{in_video, in_video + ".png"}, 0, ""));
	}

	{
		const char * args[] = {"pictura-mediocritas-tests", in_video_ext.c_str(), nullptr};
		TUPLEQ(pictura_mediocritas::options::parse(sizeof args / sizeof *args - 1, args),
		       std::make_tuple(pictura_mediocritas::options{in_video_ext, in_video + ".png"}, 0, ""));
	}

	{
		const char * args[] = {"pictura-mediocritas-tests", in_video.c_str(), out_image.c_str(), nullptr};
		TUPLEQ(pictura_mediocritas::options::parse(sizeof args / sizeof *args - 1, args),
		       std::make_tuple(pictura_mediocritas::options{in_video, out_image}, 0, ""));
	}

	{
		const char * args[] = {"pictura-mediocritas-tests", in_video_ext.c_str(), out_image.c_str(), nullptr};
		TUPLEQ(pictura_mediocritas::options::parse(sizeof args / sizeof *args - 1, args),
		       std::make_tuple(pictura_mediocritas::options{in_video_ext, out_image}, 0, ""));
	}
}
