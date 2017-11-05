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
#include <catch.hpp>


TEST_CASE("util::switch_extenstion() -- with extension", "[util]") {
	REQUIRE(pictura_mediocritas::switch_extenstion("gif_load/test.GIf", "png") == "gif_load/test.png");
	REQUIRE(pictura_mediocritas::switch_extenstion("test.png", "png") == "test.png");
	REQUIRE(pictura_mediocritas::switch_extenstion("README.md", "png") == "README.png");
}

TEST_CASE("util::switch_extenstion() -- no extension", "[util]") {
	REQUIRE(pictura_mediocritas::switch_extenstion("gif_load/test", "png") == "gif_load/test.png");
	REQUIRE(pictura_mediocritas::switch_extenstion("test", "png") == "test.png");
	REQUIRE(pictura_mediocritas::switch_extenstion("README", "png") == "README.png");
}
