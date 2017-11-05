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


#include "../test_util.hpp"
#include "util.hpp"
#include <catch.hpp>
#include <fstream>
#include <string>


using namespace std::literals;


TEST_CASE("util::read_file() -- nonexistant", "[util]") {
	const auto temp = temp_dir() + "/PicturaMediocritas/util/read_file/"s;
	make_directory_recursive(temp.c_str());

	REQUIRE(pictura_mediocritas::read_file((temp + "nonexistant_file").c_str()) == std::vector<std::uint8_t>{});
}

TEST_CASE("util::read_file() -- existant file", "[util]") {
	const auto temp = temp_dir() + "/PicturaMediocritas/util/read_file/"s;
	make_directory_recursive(temp.c_str());

	std::ofstream(temp + "file");
	REQUIRE(pictura_mediocritas::read_file((temp + "file").c_str()) == std::vector<std::uint8_t>{});

	const auto data = R"~~(
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
)~~"s;
	std::ofstream(temp + "non_empty_file", std::ios::out | std::ios::binary) << data;
	REQUIRE(pictura_mediocritas::read_file((temp + "non_empty_file").c_str()) == (std::vector<std::uint8_t>{data.begin(), data.end()}));
}
