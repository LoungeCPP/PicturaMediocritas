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


#include "util.hpp"
#include <doctest/doctest.h>
#include <fstream>
#include <string>
#include <string_view>

using namespace std::literals;


TEST_CASE("util::deduce_image_format() -- nonexistant") {
	REQUIRE(pictura_mediocritas::deduce_image_format("image.bMP"sv) == FIF_BMP);

	REQUIRE(pictura_mediocritas::deduce_image_format("image.Ico"sv) == FIF_ICO);

	REQUIRE(pictura_mediocritas::deduce_image_format("image.jPg"sv) == FIF_JPEG);
	REQUIRE(pictura_mediocritas::deduce_image_format("image.jpeg"sv) == FIF_JPEG);

	REQUIRE(pictura_mediocritas::deduce_image_format("image.jNg"sv) == FIF_JNG);

	REQUIRE(pictura_mediocritas::deduce_image_format("image.PNg"sv) == FIF_PNG);

	REQUIRE(pictura_mediocritas::deduce_image_format("image.tGa"sv) == FIF_TARGA);
	REQUIRE(pictura_mediocritas::deduce_image_format("image.tarGa"sv) == FIF_TARGA);

	REQUIRE(pictura_mediocritas::deduce_image_format("image.tIFf"sv) == FIF_TIFF);

	REQUIRE(pictura_mediocritas::deduce_image_format("image.gIF"sv) == FIF_GIF);

	REQUIRE(pictura_mediocritas::deduce_image_format("image.jpEg2000"sv) == FIF_J2K);

	REQUIRE(pictura_mediocritas::deduce_image_format("image.jP2"sv) == FIF_JP2);

	REQUIRE(pictura_mediocritas::deduce_image_format("image.WeBp"sv) == FIF_WEBP);
}

TEST_CASE("util::deduce_image_format() -- unrecognised") {
	REQUIRE(pictura_mediocritas::deduce_image_format("README.md"sv) == FIF_UNKNOWN);
	REQUIRE(pictura_mediocritas::deduce_image_format("Makefile"sv) == FIF_UNKNOWN);
}
