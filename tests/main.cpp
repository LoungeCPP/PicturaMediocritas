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


#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "test_util.hpp"
#include <cstdlib>
#include <string>

const char * temp_dir() {
	for(auto e : {"TEMP", "TMP"})
		if(const auto t = std::getenv(e))
			return t;
	return "/tmp";
}


#ifdef _WIN32

#include <windows.h>

static void make_last_dir(const char * path) {
	CreateDirectory(path, nullptr);
}

#else

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static void make_last_dir(const char * path) {
	mkdir(path, S_IRWXU);
}

#endif


// Adapted from http://stackoverflow.com/a/7430262/2851815
void make_directory_recursive(const char * path) {
	std::string tmp(path);

	if(tmp[tmp.size() - 1] == '/')
		tmp[tmp.size() - 1] = 0;
	for(char & c : tmp)
		if(c == '/') {
			c = 0;
			make_last_dir(tmp.c_str());
			c = '/';
		}
	make_last_dir(tmp.c_str());
}
