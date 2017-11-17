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


#include <array>
#include <cstddef>
#include <utility>
#include <vector>


namespace pictura_mediocritas {
	/// A representation of an average frame, continuously accepting some form of frame input.
	///
	/// The `AccT` template argument is the accumulator type.
	/// An `average_frame_u64` type alias is provided, using `uint64_t` for `AccT`,
	/// which is good for 2<sup>64</sup>b ÷ 2<sup>8</sup>bPP = 2<sup>56</sup> frames,
	/// which, at 60FPS, is good for about 2<sup>56</sup>F ÷ 60FPS ÷ 60spm ÷ 60mph ÷ 24hpd ÷ 365.25dpy ≃ 37.5 sextillion years of video.
	///
	/// If, however, you're using a deeper video, or you have a lot of frames to average, you can plug just about any type in `AccT`,
	/// as long as it has an `==`, arithmetical `+=` and `/` operators defined and a default constructor.
	///
	/// The `Channels` template argument reprensents how many channels a video has.
	template <class AccT, std::size_t Channels = 4>
	class average_frame {
		static_assert(Channels != 0, "A zero-channel video makes no sense.");

	private:
		template <class AccTL, std::size_t ChannelsL, class AccTR, std::size_t ChannelsR>
		friend bool operator==(const average_frame<AccTL, ChannelsL> & lhs, const average_frame<AccTR, ChannelsR> & rhs);


		std::size_t width;
		std::size_t height;

		AccT frames;
		std::vector<AccT> pixels;

	public:
		/// How many channels the frame has.
		constexpr static const std::size_t channels = Channels;

		/// Type of each channel value.
		using value_type = AccT;


		/// Create a frame with the specified size.
		average_frame(std::size_t width, std::size_t height);

		/// Create a frame with the specified size as `{width, height}`.
		average_frame(std::pair<std::size_t, std::size_t> size);


		/// Extract the dimensions of the frame as `{width, height}`.
		std::pair<std::size_t, std::size_t> size() const noexcept;

		/// Get how many frames were run through this frame.
		AccT processed_frames() const;

		/// Get a frame from the specified source.
		///
		/// The `frame` argument must be indexable by a `std::size_t` in [0; width * height * Channels) ∩ ℤ.
		template <class FrameT>
		void process_frame(const FrameT & frame);

		/// Get a frame from the specified source.
		///
		/// The `frame` argument must be indexable by a `std::size_t` in [0; width * height * Channels) ∩ ℤ.
		template <class FrameT>
		void process_frame(FrameT & frame);

		/// Get the average channel at the given index.
		///
		/// Special case: if no frames have been processed, return 0.
		///
		/// The behaviour is undefined if `idx` ∉ [0; width * height * Channels) ∩ ℤ.
		AccT operator[](std::size_t idx) const;

		/// Get the pixel at the given index.
		///
		/// Effectively groups `Channels` consecutive `operator[]` calls.
		///
		/// Special case: if no frames have been processed, return 0 on all channels.
		///
		/// The behaviour is undefined if `idx` ∉ [0; width * height) ∩ ℤ.
		std::array<AccT, Channels> pixel(std::size_t idx) const;
	};


	template <class AccTL, std::size_t ChannelsL, class AccTR, std::size_t ChannelsR>
	bool operator==(const average_frame<AccTL, ChannelsL> & lhs, const average_frame<AccTR, ChannelsR> & rhs);

	template <class AccTL, std::size_t ChannelsL, class AccTR, std::size_t ChannelsR>
	bool operator!=(const average_frame<AccTL, ChannelsL> & lhs, const average_frame<AccTR, ChannelsR> & rhs);


	using average_frame_u64 = average_frame<std::uint64_t>;
}


#include "average_frame.inc"
