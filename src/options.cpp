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
#include "util.hpp"

using namespace std::literals;


std::tuple<pictura_mediocritas::options, int, std::string> pictura_mediocritas::options::parse(int argc, const char * const * argv) {
	auto self = *argv ? *argv : "pictura-mediocritas";
	if(*argv)
		++argv;

	options ret{};
	if(!*argv)
	usage:
		return std::make_tuple(ret, 1, ("usage: "s += self) += " in-video [out-image]");
	ret.in_video = *argv++;

	if(*argv) {
		ret.out_image = *argv++;
		if(*argv)
			goto usage;
	} else
		ret.out_image = switch_extenstion(ret.in_video, "png");

	return std::make_tuple(ret, 0, ""s);
}

bool pictura_mediocritas::operator==(const options & lhs, const options & rhs) {
	return lhs.in_video == rhs.in_video && lhs.out_image == rhs.out_image;
}

bool pictura_mediocritas::operator!=(const options & lhs, const options & rhs) {
	return !(lhs == rhs);
}
