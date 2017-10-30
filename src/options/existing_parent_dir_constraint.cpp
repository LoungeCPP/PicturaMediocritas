// The MIT License (MIT)

// Copyright (c) 2017 Lounge<C++>

// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.


#include "existing_parent_dir_constraint.hpp"
#include "../util.hpp"


std::string pictura_mediocritas::existing_parent_dir_constraint::description() const {
	return "creatable file";
}

std::string pictura_mediocritas::existing_parent_dir_constraint::shortID() const {
	return arg_name;
}

bool pictura_mediocritas::existing_parent_dir_constraint::check(const std::string & value) const {
	auto last_slash_idx = value.find_last_of("\\/");
	if(last_slash_idx == std::string::npos)
		return directory_exists(".");
	else {
		return directory_exists(value.substr(0, last_slash_idx).c_str()) && last_slash_idx < value.size() - 1;
	}
}

pictura_mediocritas::existing_parent_dir_constraint::existing_parent_dir_constraint(std::string argname) : arg_name(std::move(argname)) {}

