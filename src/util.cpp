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
#include <cstring>
#include <strings.h>

using namespace std::literals;


bool pictura_mediocritas::has_extension(const std::string_view & path, const std::string_view & ext) {
	if(ext.size() + 1 >= path.size())
		return false;
	auto lastbit = path.substr(path.size() - (ext.size() + 1));
	if(lastbit[0] != '.')
		return false;
	return !strncasecmp(lastbit.data() + 1, ext.data(), ext.size());
}

std::string pictura_mediocritas::switch_extenstion(const std::string_view & path, const char * new_ext) {
	const auto dot = path.find_last_of('.');
	return (std::string{(dot == std::string::npos) ? path : path.substr(0, dot)} += '.') += new_ext;
}

FREE_IMAGE_FORMAT pictura_mediocritas::deduce_image_format(const std::string_view & path) {
	if(has_extension(path, "bmp"sv))
		return FIF_BMP;
	else if(has_extension(path, "ico"sv))
		return FIF_ICO;
	else if(has_extension(path, "jpg"sv) || has_extension(path, "jpeg"sv))
		return FIF_JPEG;
	else if(has_extension(path, "jng"sv))
		return FIF_JNG;
	else if(has_extension(path, "png"sv))
		return FIF_PNG;
	else if(has_extension(path, "tga"sv) || has_extension(path, "targa"sv))
		return FIF_TARGA;
	else if(has_extension(path, "tiff"sv))
		return FIF_TIFF;
	else if(has_extension(path, "gif"sv))
		return FIF_GIF;
	else if(has_extension(path, "jpeg2000"sv))
		return FIF_J2K;
	else if(has_extension(path, "jp2"sv))
		return FIF_JP2;
	else if(has_extension(path, "webp"sv))
		return FIF_WEBP;
	else
		return FIF_UNKNOWN;
}
