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


#pragma once


#include "average_frame.hpp"
#include <algorithm>


template <class AccT, std::size_t Channels>
constexpr const std::size_t pictura_mediocritas::average_frame<AccT, Channels>::channels;


template <class AccT, std::size_t Channels>
pictura_mediocritas::average_frame<AccT, Channels>::average_frame(std::size_t w, std::size_t h)
      : width(w), height(h), frames(0), pixels(width * height * Channels) {}

template <class AccT, std::size_t Channels>
pictura_mediocritas::average_frame<AccT, Channels>::average_frame(std::pair<std::size_t, std::size_t> s) : average_frame(s.first, s.second) {}

template <class AccT, std::size_t Channels>
std::pair<std::size_t, std::size_t> pictura_mediocritas::average_frame<AccT, Channels>::size() const noexcept {
	return {width, height};
}

template <class AccT, std::size_t Channels>
AccT pictura_mediocritas::average_frame<AccT, Channels>::processed_frames() const {
	return frames;
}

#define PROCESS_FRAME_BODY()                    \
	do {                                          \
		const auto pixels_len = pixels.size();      \
		for(std::size_t i = 0; i < pixels_len; ++i) \
			pixels[i] += frame[i];                    \
                                                \
		++frames;                                   \
	} while(0)

template <class AccT, std::size_t Channels>
template <class FrameT>
void pictura_mediocritas::average_frame<AccT, Channels>::process_frame(const FrameT & frame) {
	PROCESS_FRAME_BODY();
}

template <class AccT, std::size_t Channels>
template <class FrameT>
void pictura_mediocritas::average_frame<AccT, Channels>::process_frame(FrameT & frame) {
	PROCESS_FRAME_BODY();
}

#undef PROCESS_FRAME_BODY

template <class AccT, std::size_t Channels>
AccT pictura_mediocritas::average_frame<AccT, Channels>::operator[](std::size_t idx) const {
	if(frames == 0)
		return 0;
	else
		return pixels[idx] / frames;
}

template <class AccT, std::size_t Channels>
std::array<AccT, Channels> pictura_mediocritas::average_frame<AccT, Channels>::pixel(std::size_t idx) const {
	std::array<AccT, Channels> res{};
	if(frames != 0)
		std::transform(pixels.begin() + idx * Channels, pixels.begin() + idx * Channels + Channels, res.begin(), [&](auto && channel) { return channel / frames; });
	return res;
}


template <class AccTL, std::size_t ChannelsL, class AccTR, std::size_t ChannelsR>
bool pictura_mediocritas::operator==(const average_frame<AccTL, ChannelsL> & lhs, const average_frame<AccTR, ChannelsR> & rhs) {
	return lhs.width == rhs.width && lhs.height == rhs.height && lhs.frames == rhs.frames && lhs.pixels == rhs.pixels;
}

template <class AccTL, std::size_t ChannelsL, class AccTR, std::size_t ChannelsR>
bool pictura_mediocritas::operator!=(const average_frame<AccTL, ChannelsL> & lhs, const average_frame<AccTR, ChannelsR> & rhs) {
	return !(lhs == rhs);
}
