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


TEST_CASE("util::has_extension() -- equadistant") {
	REQUIRE(pictura_mediocritas::has_extension("gif_load/test.GIf", "GIF"));
	REQUIRE(pictura_mediocritas::has_extension("test.pnG", "PNG"));
	REQUIRE(pictura_mediocritas::has_extension("README.MD", "MD"));

	REQUIRE_FALSE(pictura_mediocritas::has_extension("gif_load/test_GIf", "gif"));
	REQUIRE_FALSE(pictura_mediocritas::has_extension("test", "gif"));
}

TEST_CASE("util::has_extension() -- interdistant") {
	REQUIRE_FALSE(pictura_mediocritas::has_extension("gif_load/test.Gf", "GIF"));
	REQUIRE_FALSE(pictura_mediocritas::has_extension("test.", "PNG"));
	REQUIRE_FALSE(pictura_mediocritas::has_extension("README.MDD", "MD"));
}
