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


#include "progressbar/progressbar.hpp"

#include "average_frame.hpp"
#include "options/options.hpp"
#include "output_image.hpp"
#include "parser/multi_image.hpp"
#include "util.hpp"
#include <FreeImage.h>
#include <cassert>
#include <iostream>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/dict.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}


int main(int argc, const char ** argv) {
	const auto opts_r = pictura_mediocritas::options::parse(argc, argv);
	if(std::get<1>(opts_r)) {
		std::cerr << std::get<2>(opts_r) << '\n';
		return std::get<1>(opts_r);
	}
	const auto opts = std::move(std::get<0>(opts_r));

#ifdef _WIN32
	CoInitialize(nullptr);
#endif

	FreeImage_Initialise();
	pictura_mediocritas::quickscope_wrapper freeimage_deinitialiser{FreeImage_DeInitialise};

	pictura_mediocritas::average_frame_u64 avg_frame(0, 0);
	// pictura_mediocritas::average_frame<std::uint64_t, 3> avg_frame(0, 0);

	if(pictura_mediocritas::has_extension(opts.in_video.c_str(), "gif")) {
		pictura_mediocritas::multi_image_parser parser(FreeImage_OpenMultiBitmap(FIF_GIF, opts.in_video.c_str(), false, true, true, GIF_LOAD256 | GIF_PLAYBACK),
		                                               decltype(avg_frame)::channels);
		pictura_mediocritas::progressbar progress("Processing " + opts.in_video, parser.length());
		avg_frame = decltype(avg_frame)(parser.size());

		for(auto i = 0u; i < parser.length(); ++i) {
			avg_frame.process_frame(parser);
			parser.next();
			progress.inc();
		}
	} else {
		av_register_all();
		avcodec_register_all();


		try {
			AVFormatContext * video_container = nullptr;
			pictura_mediocritas::quickscope_wrapper video_container_closer{[&]() { avformat_close_input(&video_container); }};
			if(avformat_open_input(&video_container, opts.in_video.c_str(), nullptr, nullptr) == 0) {
				std::cerr << video_container << '\n';
				std::cerr << video_container->iformat->name << '\n';
				std::cerr << video_container->iformat->long_name << '\n';

				AVCodec * best_codec   = nullptr;
				const auto best_stream = av_find_best_stream(video_container, AVMEDIA_TYPE_VIDEO, -1, -1, &best_codec, 0);
				switch(best_stream) {
					case AVERROR_STREAM_NOT_FOUND:
						std::cerr << "No video stream in input.\n";
						return 1;
					case AVERROR_DECODER_NOT_FOUND:
						std::cerr << "No available decoder for input stream.\n";
						return 1;
				}
				assert(!(best_stream < 0));  // https://www.ffmpeg.org/doxygen/trunk/group__lavf__decoding.html specifies the two values above as the only error cases

				std::cerr << best_stream << ": " << best_codec->name << '\n';
				std::cerr << best_stream << ": " << best_codec->long_name << '\n';
				std::cerr << best_stream << ": " << video_container->streams[best_stream]->codecpar->width << 'x'
				          << video_container->streams[best_stream]->codecpar->width << '\n';

				auto packet = av_packet_alloc();
				std::cerr << "packet=" << packet << "\n";
				pictura_mediocritas::quickscope_wrapper packet_freer{[&]() { av_packet_free(&packet); }};
				if(!packet) {
					std::cerr << "Couldn't allocate packet.\n";
					return 1;
				}

				std::cerr << "seek\n";
				if(av_seek_frame(video_container, best_stream, 0, 0) < 0) {
					std::cerr << "Couldn't seek to zeroth frame.\n";
					return 1;
				}

				const auto frame_read = av_read_frame(video_container, packet);
				std::cerr << "frame_read=" << frame_read << "\n";
				if(frame_read < 0) {
					std::cerr << "Couldn't read frame: " << frame_read << "\n";
					return 1;
				}

				auto frame = av_frame_alloc();
				std::cerr << "frame=" << frame << "\n";
				pictura_mediocritas::quickscope_wrapper frame_freer{[&]() { av_frame_free(&frame); }};
				if(!frame) {
					std::cerr << "Couldn't allocate frame.\n";
					return 1;
				}

				auto best_codec_ctx = avcodec_alloc_context3(best_codec);
				std::cerr << "best_codec_ctx=" << best_codec_ctx << "\n";
				pictura_mediocritas::quickscope_wrapper best_codec_ctx_freer{[&]() { avcodec_free_context(&best_codec_ctx); }};
				if(!best_codec_ctx) {
					std::cerr << "Couldn't codec context.\n";
					return 1;
				}

				std::cerr << "to\n";
				if(avcodec_parameters_to_context(best_codec_ctx, video_container->streams[best_stream]->codecpar)) {
					std::cerr << "Couldn't set parameters.\n";
					return 1;
				}

				std::cerr << "open\n";
				if(avcodec_open2(best_codec_ctx, best_codec, nullptr) < 0) {
					std::cerr << "Couldn't open codec.\n";
					return 1;
				}
				std::cerr << "opened " << packet->size << "\n";

				// std::vector<std::uint8_t> bytes;
				// std::vector<std::uint8_t> r;
				// std::vector<std::uint8_t> g;
				// std::vector<std::uint8_t> b;
				// std::vector<std::uint8_t> a;
				// uint8_t * rgba[4];
				// int rgba_linesizes[4];

				SwsContext * colour_conv_ctx = nullptr;
				pictura_mediocritas::quickscope_wrapper colour_conv_ctx_freer{[&]() { sws_freeContext(colour_conv_ctx); }};

				auto out_frame = av_frame_alloc();
				std::cerr << "out_frame=" << out_frame << "\n";
				pictura_mediocritas::quickscope_wrapper out_frame_freer{[&]() { av_frame_free(&out_frame); }};
				pictura_mediocritas::quickscope_wrapper out_frame_data_freer{[&]() { av_freep(&out_frame->data[0]); }};

				const auto send_packet = [&](AVPacket * pkt) {
					const auto packet_send_err = avcodec_send_packet(best_codec_ctx, pkt);
					std::cerr << "packet_send_err=";
					switch(packet_send_err) {
						case 0:
							std::cerr << "ok\n";
							return false;
						case AVERROR(EAGAIN):
							std::cerr << "AVERROR(EAGAIN)\n";
							return false;
						case AVERROR_EOF:
							std::cerr << "AVERROR_EOF\n";
							return false;
						case AVERROR(EINVAL):
							std::cerr << "AVERROR(EINVAL)\n";
							return true;
						case AVERROR(ENOMEM):
							std::cerr << "AVERROR(ENOMEM)\n";
							return true;
						default:
							std::cerr << packet_send_err << '\n';
							return true;
					}
				};

				const auto receive_frame = [&](int idx) {
					std::cerr << "receiving\n";
					int receive_frame_err;
					int i = 0;
					while((receive_frame_err = avcodec_receive_frame(best_codec_ctx, frame)) >= 0) {
						if(avg_frame.size().first == 0) {
							std::cerr << "making avg_frame\n";
							avg_frame = decltype(avg_frame)(frame->width, frame->height);
							// avg_frame = decltype(avg_frame)(504, 504);
						}
						// if(bytes.empty()) {
						//	bytes.resize(frame->width * frame->height * decltype(avg_frame)::channels * 32);
						//	r.resize(frame->height * frame->width * decltype(avg_frame)::channels);
						//	g.resize(frame->height * frame->width * decltype(avg_frame)::channels);
						//	b.resize(frame->height * frame->width * decltype(avg_frame)::channels);
						//	a.resize(frame->height * frame->width * decltype(avg_frame)::channels);
						//	rgba[0]           = bytes.data();  // r.data();
						//	rgba[1]           = nullptr;       // g.data();
						//	rgba[2]           = nullptr;       // b.data();
						//	rgba[3]           = nullptr;       // a.data();
						//	rgba_linesizes[0] = bytes.size();  // r.size();
						//	rgba_linesizes[1] = 0;             // g.size();
						//	rgba_linesizes[2] = 0;             // b.size();
						//	rgba_linesizes[3] = 0;             // a.size();
						//}
						if(out_frame->width == 0) {
							out_frame->format = AV_PIX_FMT_RGBA;
							out_frame->width  = frame->width;
							out_frame->height = frame->height;
							// std::cerr << "alloc="
							// << av_image_alloc(out_frame->data, out_frame->linesize, out_frame->width, out_frame->height,
							// static_cast<AVPixelFormat>(out_frame->format), 32)
							// << '\n';
							std::cerr << "get_buffer=" << av_frame_get_buffer(out_frame, 32) << '\n';
							std::cerr << "out_frame.linesizes={" << out_frame->linesize[0] << ", " << out_frame->linesize[1] << ", " << out_frame->linesize[2] << ", "
							          << out_frame->linesize[3] << "}\n";
						}

						colour_conv_ctx = sws_getCachedContext(colour_conv_ctx, frame->width, frame->height, static_cast<AVPixelFormat>(frame->format), out_frame->width,
						                                       out_frame->height, static_cast<AVPixelFormat>(out_frame->format), SWS_BICUBIC, nullptr, nullptr, nullptr);

						std::cerr << "Frame #" << best_codec_ctx->frame_number << '\n';
						std::cerr << "\t" << frame->width << 'x' << frame->height << '\n';
						std::cerr << "\t" << avg_frame.size().first << 'x' << avg_frame.size().second << '\n';
						// std::cerr << "\t" << bytes.size() << '\n';
						std::cerr << "\t" << frame->format << '\n';
						std::cerr << "\t" << colour_conv_ctx << '\n';
						// std::cerr << "\t-" << frame->crop_top << "; -" << frame->crop_bottom << "; -" << frame->crop_left << "; -" << frame->crop_right << '\n';
						// std::cerr << "\t" << std::hex << static_cast<unsigned int>(frame->data[0][0]) << ' ' << static_cast<unsigned int>(frame->data[0][1]) << ' '
						//           << static_cast<unsigned int>(frame->data[0][2]) << ' ' << static_cast<unsigned int>(frame->data[0][3]) << std::dec << '\n';

						const auto scale_error = sws_scale(colour_conv_ctx, frame->data, frame->linesize, 0, frame->height, out_frame->data, out_frame->linesize);
						std::cerr << "\tscale_error=" << scale_error << ' ' << scale_error * out_frame->linesize[0] << '\n';

						for(auto y = 0; y < frame->height / 2; ++y)
							for(auto x = 0; x < frame->width; ++x)
								for(auto c = 0u; c < decltype(avg_frame)::channels; ++c)
									std::swap(out_frame->data[0][(y * frame->width + x) * decltype(avg_frame)::channels + c],
									          out_frame->data[0][((frame->height - y) * frame->width + x) * decltype(avg_frame)::channels + c]);

						avg_frame.process_frame(out_frame->data[0]);
						std::cerr << "\t" << avg_frame.processed_frames() << '\n';

						switch(pictura_mediocritas::output_image(avg_frame.size(), decltype(avg_frame)::channels, avg_frame,
						                                         (opts.out_image + "-" + std::to_string(idx) + "-" + std::to_string(i++) + ".png").c_str())) {
							case pictura_mediocritas::output_image_result_t::ok:
								break;
							case pictura_mediocritas::output_image_result_t::deduction_error:
								std::cerr << "Could not find write codec for " << opts.out_image << ".\n";
								return true;
							case pictura_mediocritas::output_image_result_t::save_error:
								std::cerr << "Failed to write " << opts.out_image << ".\n";
								return true;
							case pictura_mediocritas::output_image_result_t::colour_set_error:
								return true;
						}
					}

					std::cerr << "receive_frame_err=";
					switch(receive_frame_err) {
						case AVERROR(EAGAIN):
							std::cerr << "AVERROR(EAGAIN)\n";
							return false;
						case AVERROR_EOF:
							std::cerr << "AVERROR_EOF\n";
							return false;
						case AVERROR(EINVAL):
							std::cerr << "AVERROR(EINVAL)\n";
							return true;
						default:
							std::cerr << receive_frame_err << '\n';
							return true;
					}
				};

				std::cerr << "sending\n";
				if(send_packet(packet))
					return 1;
				std::cerr << "while\n";
				for(auto i = 0u; packet->size; ++i) {
					std::cerr << "reading inwhile -- " << i << "\n";
					const auto frame_read = av_read_frame(video_container, packet);
					std::cerr << "\tframe_read=" << frame_read << "\n";
					if(frame_read < 0) {
						std::cerr << "\tCouldn't read frame: " << frame_read << "\n";
						return 1;
					}

					std::cerr << "\trecving inwhile\n";
					if(receive_frame(i))
						return 1;

					if(send_packet(packet))
						return 1;
				}

				std::cerr << "sending nullptr\n";
				if(send_packet(nullptr))
					return 1;
				std::cerr << "recving nullptr\n";
				if(receive_frame(-1))
					return 1;

				// const auto in_video = FreeImage_OpenMultiBitmap(FIF_GIF, opts.in_video.c_str(), false, true, true, GIF_LOAD256 | GIF_PLAYBACK);
				// pictura_mediocritas::quickscope_wrapper in_video_close{[&]() { FreeImage_CloseMultiBitmap(in_video); }};


				// const std::size_t pages = FreeImage_GetPageCount(in_video);
				// for(std::size_t i = 0; i < pages; ++i) {
				// 	const auto page = FreeImage_LockPage(in_video, i);
				// 	pictura_mediocritas::quickscope_wrapper page_unlock{[&]() { FreeImage_UnlockPage(in_video, page, false); }};

				// 	if(i == 0) {
				// 		const std::size_t w = FreeImage_GetWidth(page);
				// 		const std::size_t h = FreeImage_GetHeight(page);

				// 		frame = pictura_mediocritas::average_frame_u64(w, h);
				// 		bytes.resize(w * h * decltype(frame)::channels);
				// 	}

				// 	const auto size   = frame.size();
				// 	const auto width  = size.first;
				// 	const auto height = size.second;
				// 	for(auto x = 0u; x < width; ++x)
				// 		for(auto y = 0u; y < height; ++y) {
				// 			RGBQUAD colour{};
				// 			if(FreeImage_GetPixelColor(page, x, y, &colour)) {
				// 				bytes[(y * width + x) * decltype(frame)::channels]     = colour.rgbRed;
				// 				bytes[(y * width + x) * decltype(frame)::channels + 1] = colour.rgbGreen;
				// 				bytes[(y * width + x) * decltype(frame)::channels + 2] = colour.rgbBlue;
				// 				bytes[(y * width + x) * decltype(frame)::channels + 3] = colour.rgbReserved;
				// 				std::cerr << static_cast<unsigned>(colour.rgbReserved) << '\n';
				// 			} else {
				// 				std::cerr << "Failed to get pixel " << x << 'x' << y << " frame #" << i << ".\n";
				// 				return 1;
				// 			}
				// 		}

				// 	frame.process_frame(bytes);
				// }
			} else {
				std::cerr << "Could not find codec for " << opts.in_video << ".\n";
				return 1;
			}
		} catch(int i) {
			std::cerr << "catch(" << i << ")\n";
		}
	}


	std::cout << "Writing to " << opts.out_image << '\n';
	switch(pictura_mediocritas::output_image(avg_frame.size(), decltype(avg_frame)::channels, avg_frame, opts.out_image.c_str())) {
		case pictura_mediocritas::output_image_result_t::ok:
			break;
		case pictura_mediocritas::output_image_result_t::deduction_error:
			std::cerr << "Could not find write codec for " << opts.out_image << ".\n";
			return 1;
		case pictura_mediocritas::output_image_result_t::save_error:
			std::cerr << "Failed to write " << opts.out_image << ".\n";
			return 1;
		case pictura_mediocritas::output_image_result_t::colour_set_error:
			return 1;
	}
}
