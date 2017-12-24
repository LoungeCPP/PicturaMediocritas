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


#include "progressbar.hpp"


using namespace std::literals;


void pictura_mediocritas::progressbar::set_defaults(std::size_t max) {
	text_bar.max_refresh_rate(100ms);
	if(!max) {
		text_bar.show_tick = true;
		text_bar.show_bar  = false;
	}
}

pictura_mediocritas::progressbar::progressbar(const char * label, std::size_t max) : text_bar(max), os_bar(max) {
	text_bar.message(label);
	set_defaults(max);
}

pictura_mediocritas::progressbar::progressbar(const std::string & label, std::size_t max) : text_bar(max), os_bar(max) {
	text_bar.message(label);
	set_defaults(max);
}

void pictura_mediocritas::progressbar::inc() {
	++text_bar;
	os_bar.inc();
}

void pictura_mediocritas::progressbar::update(std::size_t value) {
	text_bar = value;
	os_bar.update(value);
}

void pictura_mediocritas::progressbar::finish() {
	text_bar.finish();
}
