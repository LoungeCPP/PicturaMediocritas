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


#define CATCH_CONFIG_ENABLE_PAIR_STRINGMAKER
#include <catch.hpp>

#include "average_frame.hpp"
#include <sstream>


using namespace std::literals;


namespace Catch {
	template <>
	template <class AccT>
	struct StringMaker<pictura_mediocritas::average_frame<AccT>> {
		static std::string convert(const pictura_mediocritas::average_frame<AccT> & value) {
			return "{size=" + StringMaker<decltype(value.size())>::convert(value.size()) +
			       ", frames=" + StringMaker<decltype(value.processed_frames())>::convert(value.processed_frames()) + "}";
		}
	};

	template <>
	template <class T, std::size_t N>
	struct StringMaker<std::array<T, N>> {
		static std::string convert(const std::array<T, N> & value) {
			std::stringstream oss;
			oss << "[";
			bool first = true;
			for(auto && v : value) {
				if(!first)
					oss << ", ";
				first = false;
				oss << v;
			}
			oss << "]";
			return oss.str();
		}
	};
}


TEST_CASE("pictura_mediocritas::average_frame(width, height)", "[average_frame]") {
	std::array<std::uint64_t, 3> zero{};

	pictura_mediocritas::average_frame_u64 hd(1280, 720);
	REQUIRE(hd.size() == std::pair<std::size_t, std::size_t>(1280, 720));
	REQUIRE(hd.processed_frames() == 0);
	for(auto i = 0u; i < 1280 * 720 * 3; ++i)
		REQUIRE(hd[i] == 0);
	for(auto i = 0u; i < 1280 * 720; ++i) {
		INFO(i);
		REQUIRE(hd.pixel(i) == zero);
	}

	pictura_mediocritas::average_frame_u64 fhd(1920, 1080);
	REQUIRE(fhd.size() == std::pair<std::size_t, std::size_t>(1920, 1080));
	REQUIRE(fhd.processed_frames() == 0);
	REQUIRE(hd.processed_frames() == 0);
	for(auto i = 0u; i < 1920 * 1080 * 3; ++i)
		REQUIRE(hd[i] == 0);
	for(auto i = 0u; i < 1920 * 1080; ++i)
		REQUIRE(hd.pixel(i) == zero);
}

TEST_CASE("pictura_mediocritas::average_frame(size)", "[average_frame]") {
	std::array<std::uint64_t, 3> zero{};

	pictura_mediocritas::average_frame_u64 hd(std::make_pair(1280, 720));
	REQUIRE(hd.size() == std::pair<std::size_t, std::size_t>(1280, 720));
	REQUIRE(hd.processed_frames() == 0);
	REQUIRE(hd.processed_frames() == 0);
	for(auto i = 0u; i < 1280 * 720 * 3; ++i)
		REQUIRE(hd[i] == 0);
	for(auto i = 0u; i < 1280 * 720; ++i)
		REQUIRE(hd.pixel(i) == zero);

	pictura_mediocritas::average_frame_u64 fhd(std::make_pair(1920, 1080));
	REQUIRE(fhd.size() == std::pair<std::size_t, std::size_t>(1920, 1080));
	REQUIRE(fhd.processed_frames() == 0);
	REQUIRE(hd.processed_frames() == 0);
	for(auto i = 0u; i < 1920 * 1080 * 3; ++i)
		REQUIRE(hd[i] == 0);
	for(auto i = 0u; i < 1920 * 1080; ++i)
		REQUIRE(hd.pixel(i) == zero);
}

TEST_CASE("pictura_mediocritas::average_frame::operator==()", "[average_frame]") {
	REQUIRE(pictura_mediocritas::average_frame_u64(1280, 720) == pictura_mediocritas::average_frame_u64(std::make_pair(1280, 720)));
	REQUIRE(pictura_mediocritas::average_frame_u64(1920, 1080) == pictura_mediocritas::average_frame_u64(std::make_pair(1920, 1080)));
}

TEST_CASE("pictura_mediocritas::average_frame::operator!=()", "[average_frame]") {
	REQUIRE(pictura_mediocritas::average_frame_u64(1280, 720) != pictura_mediocritas::average_frame_u64(1920, 1080));

	REQUIRE_FALSE(pictura_mediocritas::average_frame_u64(1280, 720) != pictura_mediocritas::average_frame_u64(std::make_pair(1280, 720)));
	REQUIRE_FALSE(pictura_mediocritas::average_frame_u64(1920, 1080) != pictura_mediocritas::average_frame_u64(std::make_pair(1920, 1080)));
}

TEST_CASE("pictura_mediocritas::average_frame::processed_frames()", "[average_frame]") {
	pictura_mediocritas::average_frame<std::uint64_t, 1> frame(1, 1);
	for(int i = 0; i < 10; ++i) {
		REQUIRE(frame.processed_frames() == i);
		const std::uint8_t f[] = {static_cast<std::uint8_t>('0' + i)};
		frame.process_frame(f);
		REQUIRE(frame.processed_frames() == i + 1);
	}
}

TEST_CASE("pictura_mediocritas::average_frame::operator[]()", "[average_frame]") {
	pictura_mediocritas::average_frame<std::uint64_t, 1> frame(1, 1);
	REQUIRE(frame[0] == 0);

	auto acc = 0u;
	for(auto i = 0u; i < 256; ++i) {
		acc += i;

		const std::uint8_t f[] = {static_cast<std::uint8_t>(i)};
		frame.process_frame(f);
		REQUIRE(frame[0] == acc / (i + 1));
	}
}

TEST_CASE("pictura_mediocritas::average_frame::pixel()", "[average_frame]") {
	pictura_mediocritas::average_frame<std::uint64_t, 2> frame(1, 1);
	REQUIRE(frame.pixel(0) == (std::array<std::uint64_t, 2>{0, 0}));

	auto acc_inc = 0u;
	auto acc_dec = 0u;
	for(auto i = 0u; i < 256; ++i) {
		acc_inc += i;
		acc_dec += 255 - i;

		const std::uint8_t f[] = {static_cast<std::uint8_t>(i), static_cast<std::uint8_t>(255 - i)};
		frame.process_frame(f);
		REQUIRE(frame.pixel(0) == (std::array<std::uint64_t, 2>{acc_inc / (i + 1), acc_dec / (i + 1)}));
	}
}
