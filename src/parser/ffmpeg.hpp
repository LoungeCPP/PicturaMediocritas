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


#pragma once


#include <cstdint>
#include <functional>
#include <memory>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}


namespace pictura_mediocritas {
	struct av_format_context_deleter {
		void operator()(AVFormatContext * ctx) const noexcept;
	};

	struct av_packet_deleter {
		void operator()(AVPacket * pkt) const noexcept;
	};

	struct av_frame_deleter {
		void operator()(AVFrame * frm) const noexcept;
	};

	struct av_codec_context_deleter {
		void operator()(AVCodecContext * ctx) const noexcept;
	};

	struct sws_context_deleter {
		void operator()(SwsContext * ctx) const noexcept;
	};

	class ffmpeg_parser {
	public:
		enum class error_class_t : std::uint8_t {
			none,
			open_input,
			find_best_stream,
			seek,
			set_codec_parameters,
			open_codec,
			send_packet,
			receive_frame,
			channel_count,
			get_frame_buffer,
			scale,
			read_frame,
		};


		std::unique_ptr<AVFormatContext, av_format_context_deleter> container;
		std::unique_ptr<AVPacket, av_packet_deleter> packet;
		std::unique_ptr<AVFrame, av_frame_deleter> orig_frame;
		std::unique_ptr<AVCodecContext, av_codec_context_deleter> best_codec_ctx;
		std::unique_ptr<SwsContext, sws_context_deleter> colour_conv_ctx;
		std::unique_ptr<AVFrame, av_frame_deleter> out_frame;

		int best_stream;
		AVCodec * best_codec;
		std::size_t channels;

		error_class_t error_class;
		int error_value;

		bool send_packet(AVPacket * pkt) noexcept;
		bool receive_frame(const std::function<bool()> & callback) noexcept;


	public:
		ffmpeg_parser(const char * filename, std::size_t channels);


		/// Check if this parser is in a valid state.
		explicit operator bool() const noexcept;

		/// Get the error string, or `nullptr` if conversion to bool is `true`.
		const char * error() const noexcept;

		/// Get current frame's size as `{width, height}`, or `{0, 0}` if noty yet ready.
		std::pair<std::size_t, std::size_t> size() const noexcept;

		/// Get frame count, or `0` if not yet ready.
		std::size_t length() const noexcept;

		/// Run through every frame, calling `callback()` after initialising it.
		bool process(const std::function<bool()> & callback);

		/// Get the specified colour fragment.
		///
		/// Valid inside [0; width * height * channels) ∩ ℤ, undefined behaviour thereoutside.
		///
		/// `idx` is in the format `(y * width + x) * decltype(frame)::channels + channel`
		/// (i.e. the one required by `average_frame`).
		std::uint8_t operator[](std::size_t idx) const noexcept;
	};
}
