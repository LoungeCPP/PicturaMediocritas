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


#include "average_frame.hpp"
#include "options.hpp"
#include "output_image.hpp"
#include "parser/ffmpeg.hpp"
#include "parser/multi_image.hpp"
#include "util.hpp"
#include <FreeImage.h>
#include <atomic>
#include <cassert>
#include <iostream>
#include <pthread.h>
#include <thread>
#include <unistd.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

using namespace std::chrono_literals;


#define STATUSSY(donetest, curframe)                                                                                                   \
	const auto start = std::chrono::high_resolution_clock::now();                                                                        \
	std::jthread statussy {                                                                                                              \
		[&] {                                                                                                                              \
			auto write = [&](std::size_t done) {                                                                                             \
				const auto now = std::chrono::high_resolution_clock::now();                                                                    \
				const auto len = parser.length();                                                                                              \
				std::fprintf(stderr, "\r%*zu/%zu\t%.4f/s", (int)std::log10(len | 1) + 1, done, len,                                            \
				             (done / static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count()) * 1000)); \
			};                                                                                                                               \
                                                                                                                                       \
			for(;;) {                                                                                                                        \
				for(int _ = 0; _ < 10; ++_) {                                                                                                  \
					std::this_thread::sleep_for(100ms);                                                                                          \
					if(donetest)                                                                                                                 \
						goto done;                                                                                                                 \
				}                                                                                                                              \
				if(isatty(2))                                                                                                                  \
					write(curframe);                                                                                                             \
			}                                                                                                                                \
		done:                                                                                                                              \
			write(parser.length() ? parser.length() : curframe);                                                                             \
			std::fputc('\n', stderr);                                                                                                        \
		}                                                                                                                                  \
	}


int main(int argc, const char ** argv) {
	const auto opts_r = pictura_mediocritas::options::parse(argc, argv);
	if(std::get<1>(opts_r)) {
		std::cerr << std::get<2>(opts_r) << '\n';
		return std::get<1>(opts_r);
	}
	const auto opts = std::move(std::get<0>(opts_r));


	FreeImage_Initialise();
	pictura_mediocritas::quickscope_wrapper freeimage_deinitialiser{FreeImage_DeInitialise};

#define MAXTHREADS 8u
	pictura_mediocritas::average_frame_u64 avg_frame(0, 0);
	auto thread_cnt = std::clamp(std::thread::hardware_concurrency(), 1u, MAXTHREADS);

	if(pictura_mediocritas::has_extension(opts.in_video.data(), "gif")) {
		pictura_mediocritas::multi_image_parser parser(FreeImage_OpenMultiBitmap(FIF_GIF, opts.in_video.data(), false, true, true, GIF_LOAD256 | GIF_PLAYBACK),
		                                               decltype(avg_frame)::channels);
		avg_frame     = decltype(avg_frame)(parser.size());
		std::size_t i = 0;
		STATUSSY(i == parser.length(), i);
		for(; i < parser.length(); ++i) {
			avg_frame.process_frame(parser);
			parser.next();
		}
	} else {
		pictura_mediocritas::ffmpeg_parser parser(opts.in_video.data(), decltype(avg_frame)::channels, thread_cnt);
		if(parser) {
			struct thread {
				pictura_mediocritas::average_frame_u64 avg_frame;
				std::thread thread;
				pthread_barrier_t barrier;
				std::atomic<std::size_t> cur_frame_num;
				std::atomic_flag done;
			};
			thread threads[MAXTHREADS] = {{{0, 0}}, {{0, 0}}, {{0, 0}}, {{0, 0}}, {{0, 0}}, {{0, 0}}, {{0, 0}}, {{0, 0}}};
			STATUSSY(threads[0].done.test(), threads[0].cur_frame_num.load(std::memory_order_relaxed));
			if(!parser.process([&]() {
				   if(avg_frame.size().first == 0) {
					   avg_frame = decltype(avg_frame)(parser.size());

					   for(auto i = 0u; i < thread_cnt; ++i) {
						   threads[i].avg_frame = decltype(avg_frame)(parser.size());
						   while(pthread_barrier_init(&threads[i].barrier, nullptr, 2))
							   ;
						   threads[i].thread = std::thread{[&, i = i] {
							   auto & self = threads[i];

							   pthread_barrier_wait(&self.barrier);
							   for(;;) {
								   pthread_barrier_wait(&self.barrier);
								   if(self.done.test())
									   break;
								   auto frame = self.cur_frame_num.load(std::memory_order_relaxed);
								   self.avg_frame.process_frame(parser, frame);
								   pthread_barrier_wait(&self.barrier);
							   }
						   }};
					   }
				   }

				   auto & thread = threads[parser.frame_num % thread_cnt];
				   pthread_barrier_wait(&thread.barrier);
				   thread.cur_frame_num.store(parser.frame_num, std::memory_order_relaxed);
				   pthread_barrier_wait(&thread.barrier);

				   return true;
			   })) {
				std::cerr << "Parsing " << opts.in_video << " failed: " << *parser.error() << '\n';
				std::exit(1);
			} else {
				for(auto i = 0u; i < thread_cnt; ++i) {
					threads[i].done.test_and_set();
					pthread_barrier_wait(&threads[i].barrier);
					pthread_barrier_wait(&threads[i].barrier);
					threads[i].thread.join();
					avg_frame += threads[i].avg_frame;
				}
			}

			parser.postprocess(avg_frame);
		} else if(parser.error() == "") {
			std::cerr << "Couldn't open " << opts.in_video << ".\n";
			return 1;
		} else {
			std::cerr << "Could not find codec for " << opts.in_video << ": " << *parser.error() << '\n';
			return 1;
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
