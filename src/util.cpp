// The MIT License (MIT)

// Copyright (c) 2017 nabijaczleweli

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


#include "util.hpp"
#include <algorithm>
#include <cctype>
#include <cstring>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>


pictura_mediocritas::quickscope_wrapper::~quickscope_wrapper() {
	if(func)
		func();
}


bool pictura_mediocritas::file_exists(const char * path) {
	struct stat info;
	return stat(path, &info) == 0 && info.st_mode & S_IFREG;
}

bool pictura_mediocritas::directory_exists(const char * path) {
	struct stat info;
	return stat(path, &info) == 0 && info.st_mode & S_IFDIR;
}

bool pictura_mediocritas::has_extension(const char * path, const char * ext) {
	const auto path_len = std::strlen(path);
	auto path_ext       = path + path_len;
	while(*(path_ext - 1) != '.' && path_ext - 1 != path)
		--path_ext;
	const auto path_ext_len = std::strlen(path_ext);

	const auto ext_len = std::strlen(ext);

	if(path_ext_len == ext_len) {
		for(auto i = 0u; i < ext_len; ++i)
			if(std::tolower(ext[i]) != std::tolower(path_ext[i]))
				return false;

		return true;
	} else
		return false;
}

std::string pictura_mediocritas::switch_extenstion(const std::string & path, const char * new_ext) {
	const auto dot = path.find_last_of('.');
	if(dot == std::string::npos)
		return path + '.' + new_ext;
	else
		return path.substr(0, dot) + '.' + new_ext;
}

FREE_IMAGE_FORMAT pictura_mediocritas::deduce_image_format(const char * path) {
	if(has_extension(path, "bmp"))
		return FIF_BMP;
	else if(has_extension(path, "ico"))
		return FIF_ICO;
	else if(has_extension(path, "jpg") || has_extension(path, "jpeg"))
		return FIF_JPEG;
	else if(has_extension(path, "jng"))
		return FIF_JNG;
	else if(has_extension(path, "png"))
		return FIF_PNG;
	else if(has_extension(path, "tga") || has_extension(path, "targa"))
		return FIF_TARGA;
	else if(has_extension(path, "tiff"))
		return FIF_TIFF;
	else if(has_extension(path, "gif"))
		return FIF_GIF;
	else if(has_extension(path, "jpeg2000"))
		return FIF_J2K;
	else if(has_extension(path, "jp2"))
		return FIF_JP2;
	else if(has_extension(path, "webp"))
		return FIF_WEBP;
	else
		return FIF_UNKNOWN;
}

std::vector<std::uint8_t> pictura_mediocritas::read_file(const char * path) {
	std::vector<std::uint8_t> ret;

	std::ifstream in_file(path, std::ios::in | std::ios::binary);
	std::uint8_t buf[1024];
	in_file.read(static_cast<char *>(static_cast<void *>(buf)), sizeof buf / sizeof *buf);
	while(in_file.gcount()) {
		ret.insert(ret.end(), buf, buf + in_file.gcount());
		in_file.read(static_cast<char *>(static_cast<void *>(buf)), sizeof buf / sizeof *buf);
	}

	return ret;
}
