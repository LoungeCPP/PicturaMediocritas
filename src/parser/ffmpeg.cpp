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


#include "ffmpeg.hpp"


void pictura_mediocritas::av_format_context_deleter::operator()(AVFormatContext * ctx) const noexcept {
	avformat_close_input(&ctx);
}

void pictura_mediocritas::av_packet_deleter::operator()(AVPacket * pkt) const noexcept {
	av_packet_free(&pkt);
}

void pictura_mediocritas::av_frame_deleter::operator()(AVFrame * frm) const noexcept {
	av_frame_free(&frm);
}

void pictura_mediocritas::av_codec_context_deleter::operator()(AVCodecContext * ctx) const noexcept {
	avcodec_free_context(&ctx);
}

void pictura_mediocritas::sws_context_deleter::operator()(SwsContext * ctx) const noexcept {
	sws_freeContext(ctx);
}


bool pictura_mediocritas::ffmpeg_parser::send_packet(AVPacket * pkt) noexcept {
	const auto err = avcodec_send_packet(best_codec_ctx.get(), pkt);
	switch(err) {
		case 0:
		case AVERROR(EAGAIN):
		case AVERROR_EOF:
			return false;
		default:
			error_value = err;
			error_class = error_class_t::send_packet;
			return true;
	}
}

bool pictura_mediocritas::ffmpeg_parser::receive_frame(const std::function<bool()> & callback) noexcept {
	while((error_value = avcodec_receive_frame(best_codec_ctx.get(), orig_frame.get())) >= 0) {
		if(out_frame->width == 0) {
			out_frame->width  = orig_frame->width;
			out_frame->height = orig_frame->height;

			const auto get_buffer_error = av_frame_get_buffer(out_frame.get(), 32);
			if(get_buffer_error < 0) {
				error_class = error_class_t::get_frame_buffer;
				error_value = get_buffer_error;
				return true;
			}
		}

		// sws_getCachedContext() will free colour_conv_ctx by itself, so we need to be very careful not to do it ourselves,
		// so we release it first, *then* reset it, otherwise a whole new world of doublefree opens up.
		const auto new_colour_conv_ctx =
		    sws_getCachedContext(colour_conv_ctx.get(), orig_frame->width, orig_frame->height, static_cast<AVPixelFormat>(orig_frame->format), out_frame->width,
		                         out_frame->height, static_cast<AVPixelFormat>(out_frame->format), SWS_BICUBIC, nullptr, nullptr, nullptr);
		if(new_colour_conv_ctx != colour_conv_ctx.get()) {
			colour_conv_ctx.release();
			colour_conv_ctx.reset(new_colour_conv_ctx);
		}

		const auto scale_error =
		    sws_scale(colour_conv_ctx.get(), orig_frame->data, orig_frame->linesize, 0, orig_frame->height, out_frame->data, out_frame->linesize);
		if(scale_error <= 0) {
			error_value = scale_error - 1;
			error_class = error_class_t::scale;
			return true;
		}

		// TODO: is there a more performant way of flipping the video?
		for(auto y = 0; y < out_frame->height / 2; ++y)
			for(auto x = 0; x < out_frame->width; ++x)
				for(auto c = 0u; c < channels; ++c)
					std::swap(out_frame->data[0][(y * out_frame->width + x) * channels + c],
					          out_frame->data[0][((out_frame->height - 1 - y) * out_frame->width + x) * channels + c]);

		if(!callback())
			return true;
	}

	switch(error_value) {
		case AVERROR(EAGAIN):
		case AVERROR_EOF:
			return false;
		default:
			error_class = error_class_t::receive_frame;
			return true;
	}
}

pictura_mediocritas::ffmpeg_parser::ffmpeg_parser(const char * filename, std::size_t c)
      : best_stream(-69), best_codec(nullptr), channels(c), error_class(error_class_t::none), error_value(0) {
	AVFormatContext * container_in = nullptr;
	if((error_value = avformat_open_input(&container_in, filename, nullptr, nullptr)) != 0) {
		error_class = error_class_t::open_input;
		return;
	}
	container.reset(container_in);

	if((error_value = best_stream = av_find_best_stream(container.get(), AVMEDIA_TYPE_VIDEO, -1, -1, &best_codec, 0)) < 0) {
		error_class = error_class_t::find_best_stream;
		return;
	}

	packet.reset(av_packet_alloc());
	if(!packet)
		return;

	// if((error_value = av_seek_frame(container.get(), best_stream, 0, 0)) < 0) {
	// 	error_class = error_class_t::seek;
	// 	return;
	// }

	orig_frame.reset(av_frame_alloc());
	if(!orig_frame)
		return;

	best_codec_ctx.reset(avcodec_alloc_context3(best_codec));
	if(!best_codec_ctx)
		return;

	if((error_value = avcodec_parameters_to_context(best_codec_ctx.get(), container->streams[best_stream]->codecpar)) < 0) {
		error_class = error_class_t::set_codec_parameters;
		return;
	}

	if((error_value = avcodec_open2(best_codec_ctx.get(), best_codec, nullptr)) < 0) {
		error_class = error_class_t::set_codec_parameters;
		return;
	}

	out_frame.reset(av_frame_alloc());
	if(!out_frame)
		return;

	switch(channels) {
		case 1:
			out_frame->format = AV_PIX_FMT_GRAY8;
			break;
		case 3:
			out_frame->format = AV_PIX_FMT_RGB24;
			break;
		case 4:
			out_frame->format = AV_PIX_FMT_RGBA;
			break;
		default:
			error_value = -static_cast<int>(channels) - 1;
			error_class = error_class_t::set_codec_parameters;
			return;
	}
}

pictura_mediocritas::ffmpeg_parser::operator bool() const noexcept {
	return packet && orig_frame && best_codec_ctx && out_frame && (error_class == error_class_t::none && error_value >= 0);
}

const char * pictura_mediocritas::ffmpeg_parser::error() const noexcept {
	switch(error_class) {
		case error_class_t::none:
			break;

		case error_class_t::open_input:
			return "";

		case error_class_t::find_best_stream:
			switch(error_value) {
				case AVERROR_STREAM_NOT_FOUND:
					return "No video stream in input.";

				case AVERROR_DECODER_NOT_FOUND:
					return "No available decoder for input stream.";

				default:
					// Unreachable
					break;
			}

		case error_class_t::seek:
			return "Couldn't seek to zeroth frame.";

		case error_class_t::set_codec_parameters:
			return "Couldn't set codec context parameters.";

		case error_class_t::open_codec:
			return "Couldn't open codec.";

		case error_class_t::send_packet:
			switch(error_value) {
				case AVERROR(EINVAL):
					return "Couldn't send packet – invalid video.";

				case AVERROR(ENOMEM):
					return "Couldn't send packet – out of memory.";

				default:
					return "Couldn't send packet.";
			}

		case error_class_t::receive_frame:
			switch(error_value) {
				case AVERROR(EINVAL):
					return "Couldn't receive frame – invalid video.";

				default:
					return "Couldn't receive frame.";
			}

		case error_class_t::channel_count:
			switch(channels) {
				case 0:
					return "Zero-channel video makes no sense.";

				default:
					return "Invalid channel count.";
			}

		case error_class_t::read_frame:
			return "Couldn't read frame.";

		case error_class_t::get_frame_buffer:
			return "Couldn't get output frame buffer.";

		case error_class_t::scale:
			return "Couldn't scale image.";
	}

	if(!packet)
		return "Couldn't allocate packet.";

	if(!orig_frame)
		return "Couldn't allocate frame.";

	if(!best_codec_ctx)
		return "Couldn't allocate codec context.";

	if(!out_frame)
		return "Couldn't allocate output frame.";

	return nullptr;
}

std::pair<std::size_t, std::size_t> pictura_mediocritas::ffmpeg_parser::size() const noexcept {
	if(orig_frame)
		return {orig_frame->width, orig_frame->height};
	else
		return {0, 0};
}

std::size_t pictura_mediocritas::ffmpeg_parser::length() const noexcept {
	if(container && container->streams[best_stream])
		return container->streams[best_stream]->nb_frames;
	else
		return 0;
}

bool pictura_mediocritas::ffmpeg_parser::process(const std::function<bool()> & callback) {
	if(!*this)
		return false;

	do {
		if((error_value = av_read_frame(container.get(), packet.get())) < 0)
			return false;

		if(receive_frame(callback))
			return false;

		if(send_packet(packet.get()))
			return false;
	} while(packet->size);

	if(send_packet(nullptr))
		return false;

	if(receive_frame(callback))
		return false;

	return true;
}

std::uint8_t pictura_mediocritas::ffmpeg_parser::operator[](std::size_t idx) const noexcept {
	if(out_frame && out_frame->data[0])
		return out_frame->data[0][idx];
	else
		return -1;
}
