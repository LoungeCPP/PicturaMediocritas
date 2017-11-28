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
}


static void pick_pixel3(AVFrame * frame, int x, int y, int * s0, int * s1, int * s2) {
	if(s0)
		*s0 = frame->data[0][y * frame->linesize[0] + x];
	if(s1)
		*s1 = frame->data[1][y * frame->linesize[1] + x];
	if(s2)
		*s2 = frame->data[2][y * frame->linesize[2] + x];
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

	pictura_mediocritas::average_frame<std::uint64_t, 3> avg_frame(0, 0);

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


		std::cerr << "AV_PIX_FMT_NONE: " << AV_PIX_FMT_NONE << '\n';
		std::cerr << "AV_PIX_FMT_YUV420P: " << AV_PIX_FMT_YUV420P << '\n';
		std::cerr << "AV_PIX_FMT_YUYV422: " << AV_PIX_FMT_YUYV422 << '\n';
		std::cerr << "AV_PIX_FMT_RGB24: " << AV_PIX_FMT_RGB24 << '\n';
		std::cerr << "AV_PIX_FMT_BGR24: " << AV_PIX_FMT_BGR24 << '\n';
		std::cerr << "AV_PIX_FMT_YUV422P: " << AV_PIX_FMT_YUV422P << '\n';
		std::cerr << "AV_PIX_FMT_YUV444P: " << AV_PIX_FMT_YUV444P << '\n';
		std::cerr << "AV_PIX_FMT_YUV410P: " << AV_PIX_FMT_YUV410P << '\n';
		std::cerr << "AV_PIX_FMT_YUV411P: " << AV_PIX_FMT_YUV411P << '\n';
		std::cerr << "AV_PIX_FMT_GRAY8: " << AV_PIX_FMT_GRAY8 << '\n';
		std::cerr << "AV_PIX_FMT_MONOWHITE: " << AV_PIX_FMT_MONOWHITE << '\n';
		std::cerr << "AV_PIX_FMT_MONOBLACK: " << AV_PIX_FMT_MONOBLACK << '\n';
		std::cerr << "AV_PIX_FMT_PAL8: " << AV_PIX_FMT_PAL8 << '\n';
		std::cerr << "AV_PIX_FMT_YUVJ420P: " << AV_PIX_FMT_YUVJ420P << '\n';
		std::cerr << "AV_PIX_FMT_YUVJ422P: " << AV_PIX_FMT_YUVJ422P << '\n';
		std::cerr << "AV_PIX_FMT_YUVJ444P: " << AV_PIX_FMT_YUVJ444P << '\n';
		std::cerr << "AV_PIX_FMT_UYVY422: " << AV_PIX_FMT_UYVY422 << '\n';
		std::cerr << "AV_PIX_FMT_UYYVYY411: " << AV_PIX_FMT_UYYVYY411 << '\n';
		std::cerr << "AV_PIX_FMT_BGR8: " << AV_PIX_FMT_BGR8 << '\n';
		std::cerr << "AV_PIX_FMT_BGR4: " << AV_PIX_FMT_BGR4 << '\n';
		std::cerr << "AV_PIX_FMT_BGR4_BYTE: " << AV_PIX_FMT_BGR4_BYTE << '\n';
		std::cerr << "AV_PIX_FMT_RGB8: " << AV_PIX_FMT_RGB8 << '\n';
		std::cerr << "AV_PIX_FMT_RGB4: " << AV_PIX_FMT_RGB4 << '\n';
		std::cerr << "AV_PIX_FMT_RGB4_BYTE: " << AV_PIX_FMT_RGB4_BYTE << '\n';
		std::cerr << "AV_PIX_FMT_NV12: " << AV_PIX_FMT_NV12 << '\n';
		std::cerr << "AV_PIX_FMT_NV21: " << AV_PIX_FMT_NV21 << '\n';
		std::cerr << "AV_PIX_FMT_ARGB: " << AV_PIX_FMT_ARGB << '\n';
		std::cerr << "AV_PIX_FMT_RGBA: " << AV_PIX_FMT_RGBA << '\n';
		std::cerr << "AV_PIX_FMT_ABGR: " << AV_PIX_FMT_ABGR << '\n';
		std::cerr << "AV_PIX_FMT_BGRA: " << AV_PIX_FMT_BGRA << '\n';
		std::cerr << "AV_PIX_FMT_GRAY16BE: " << AV_PIX_FMT_GRAY16BE << '\n';
		std::cerr << "AV_PIX_FMT_GRAY16LE: " << AV_PIX_FMT_GRAY16LE << '\n';
		std::cerr << "AV_PIX_FMT_YUV440P: " << AV_PIX_FMT_YUV440P << '\n';
		std::cerr << "AV_PIX_FMT_YUVJ440P: " << AV_PIX_FMT_YUVJ440P << '\n';
		std::cerr << "AV_PIX_FMT_YUVA420P: " << AV_PIX_FMT_YUVA420P << '\n';
		std::cerr << "AV_PIX_FMT_RGB48BE: " << AV_PIX_FMT_RGB48BE << '\n';
		std::cerr << "AV_PIX_FMT_RGB48LE: " << AV_PIX_FMT_RGB48LE << '\n';
		std::cerr << "AV_PIX_FMT_RGB565BE: " << AV_PIX_FMT_RGB565BE << '\n';
		std::cerr << "AV_PIX_FMT_RGB565LE: " << AV_PIX_FMT_RGB565LE << '\n';
		std::cerr << "AV_PIX_FMT_RGB555BE: " << AV_PIX_FMT_RGB555BE << '\n';
		std::cerr << "AV_PIX_FMT_RGB555LE: " << AV_PIX_FMT_RGB555LE << '\n';
		std::cerr << "AV_PIX_FMT_BGR565BE: " << AV_PIX_FMT_BGR565BE << '\n';
		std::cerr << "AV_PIX_FMT_BGR565LE: " << AV_PIX_FMT_BGR565LE << '\n';
		std::cerr << "AV_PIX_FMT_BGR555BE: " << AV_PIX_FMT_BGR555BE << '\n';
		std::cerr << "AV_PIX_FMT_BGR555LE: " << AV_PIX_FMT_BGR555LE << '\n';
		std::cerr << "AV_PIX_FMT_VAAPI_MOCO: " << AV_PIX_FMT_VAAPI_MOCO << '\n';
		std::cerr << "AV_PIX_FMT_VAAPI_IDCT: " << AV_PIX_FMT_VAAPI_IDCT << '\n';
		std::cerr << "AV_PIX_FMT_VAAPI_VLD: " << AV_PIX_FMT_VAAPI_VLD << '\n';
		std::cerr << "AV_PIX_FMT_VAAPI: " << AV_PIX_FMT_VAAPI << '\n';
		std::cerr << "AV_PIX_FMT_YUV420P16LE: " << AV_PIX_FMT_YUV420P16LE << '\n';
		std::cerr << "AV_PIX_FMT_YUV420P16BE: " << AV_PIX_FMT_YUV420P16BE << '\n';
		std::cerr << "AV_PIX_FMT_YUV422P16LE: " << AV_PIX_FMT_YUV422P16LE << '\n';
		std::cerr << "AV_PIX_FMT_YUV422P16BE: " << AV_PIX_FMT_YUV422P16BE << '\n';
		std::cerr << "AV_PIX_FMT_YUV444P16LE: " << AV_PIX_FMT_YUV444P16LE << '\n';
		std::cerr << "AV_PIX_FMT_YUV444P16BE: " << AV_PIX_FMT_YUV444P16BE << '\n';
		std::cerr << "AV_PIX_FMT_DXVA2_VLD: " << AV_PIX_FMT_DXVA2_VLD << '\n';
		std::cerr << "AV_PIX_FMT_RGB444LE: " << AV_PIX_FMT_RGB444LE << '\n';
		std::cerr << "AV_PIX_FMT_RGB444BE: " << AV_PIX_FMT_RGB444BE << '\n';
		std::cerr << "AV_PIX_FMT_BGR444LE: " << AV_PIX_FMT_BGR444LE << '\n';
		std::cerr << "AV_PIX_FMT_BGR444BE: " << AV_PIX_FMT_BGR444BE << '\n';
		std::cerr << "AV_PIX_FMT_YA8: " << AV_PIX_FMT_YA8 << '\n';
		std::cerr << "AV_PIX_FMT_Y400A: " << AV_PIX_FMT_Y400A << '\n';
		std::cerr << "AV_PIX_FMT_GRAY8A: " << AV_PIX_FMT_GRAY8A << '\n';
		std::cerr << "AV_PIX_FMT_BGR48BE: " << AV_PIX_FMT_BGR48BE << '\n';
		std::cerr << "AV_PIX_FMT_BGR48LE: " << AV_PIX_FMT_BGR48LE << '\n';
		std::cerr << "AV_PIX_FMT_YUV420P9BE: " << AV_PIX_FMT_YUV420P9BE << '\n';
		std::cerr << "AV_PIX_FMT_YUV420P9LE: " << AV_PIX_FMT_YUV420P9LE << '\n';
		std::cerr << "AV_PIX_FMT_YUV420P10BE: " << AV_PIX_FMT_YUV420P10BE << '\n';
		std::cerr << "AV_PIX_FMT_YUV420P10LE: " << AV_PIX_FMT_YUV420P10LE << '\n';
		std::cerr << "AV_PIX_FMT_YUV422P10BE: " << AV_PIX_FMT_YUV422P10BE << '\n';
		std::cerr << "AV_PIX_FMT_YUV422P10LE: " << AV_PIX_FMT_YUV422P10LE << '\n';
		std::cerr << "AV_PIX_FMT_YUV444P9BE: " << AV_PIX_FMT_YUV444P9BE << '\n';
		std::cerr << "AV_PIX_FMT_YUV444P9LE: " << AV_PIX_FMT_YUV444P9LE << '\n';
		std::cerr << "AV_PIX_FMT_YUV444P10BE: " << AV_PIX_FMT_YUV444P10BE << '\n';
		std::cerr << "AV_PIX_FMT_YUV444P10LE: " << AV_PIX_FMT_YUV444P10LE << '\n';
		std::cerr << "AV_PIX_FMT_YUV422P9BE: " << AV_PIX_FMT_YUV422P9BE << '\n';
		std::cerr << "AV_PIX_FMT_YUV422P9LE: " << AV_PIX_FMT_YUV422P9LE << '\n';
		std::cerr << "AV_PIX_FMT_GBRP: " << AV_PIX_FMT_GBRP << '\n';
		std::cerr << "AV_PIX_FMT_GBR24P: " << AV_PIX_FMT_GBR24P << '\n';
		std::cerr << "AV_PIX_FMT_GBRP9BE: " << AV_PIX_FMT_GBRP9BE << '\n';
		std::cerr << "AV_PIX_FMT_GBRP9LE: " << AV_PIX_FMT_GBRP9LE << '\n';
		std::cerr << "AV_PIX_FMT_GBRP10BE: " << AV_PIX_FMT_GBRP10BE << '\n';
		std::cerr << "AV_PIX_FMT_GBRP10LE: " << AV_PIX_FMT_GBRP10LE << '\n';
		std::cerr << "AV_PIX_FMT_GBRP16BE: " << AV_PIX_FMT_GBRP16BE << '\n';
		std::cerr << "AV_PIX_FMT_GBRP16LE: " << AV_PIX_FMT_GBRP16LE << '\n';
		std::cerr << "AV_PIX_FMT_YUVA422P: " << AV_PIX_FMT_YUVA422P << '\n';
		std::cerr << "AV_PIX_FMT_YUVA444P: " << AV_PIX_FMT_YUVA444P << '\n';
		std::cerr << "AV_PIX_FMT_YUVA420P9BE: " << AV_PIX_FMT_YUVA420P9BE << '\n';
		std::cerr << "AV_PIX_FMT_YUVA420P9LE: " << AV_PIX_FMT_YUVA420P9LE << '\n';
		std::cerr << "AV_PIX_FMT_YUVA422P9BE: " << AV_PIX_FMT_YUVA422P9BE << '\n';
		std::cerr << "AV_PIX_FMT_YUVA422P9LE: " << AV_PIX_FMT_YUVA422P9LE << '\n';
		std::cerr << "AV_PIX_FMT_YUVA444P9BE: " << AV_PIX_FMT_YUVA444P9BE << '\n';
		std::cerr << "AV_PIX_FMT_YUVA444P9LE: " << AV_PIX_FMT_YUVA444P9LE << '\n';
		std::cerr << "AV_PIX_FMT_YUVA420P10BE: " << AV_PIX_FMT_YUVA420P10BE << '\n';
		std::cerr << "AV_PIX_FMT_YUVA420P10LE: " << AV_PIX_FMT_YUVA420P10LE << '\n';
		std::cerr << "AV_PIX_FMT_YUVA422P10BE: " << AV_PIX_FMT_YUVA422P10BE << '\n';
		std::cerr << "AV_PIX_FMT_YUVA422P10LE: " << AV_PIX_FMT_YUVA422P10LE << '\n';
		std::cerr << "AV_PIX_FMT_YUVA444P10BE: " << AV_PIX_FMT_YUVA444P10BE << '\n';
		std::cerr << "AV_PIX_FMT_YUVA444P10LE: " << AV_PIX_FMT_YUVA444P10LE << '\n';
		std::cerr << "AV_PIX_FMT_YUVA420P16BE: " << AV_PIX_FMT_YUVA420P16BE << '\n';
		std::cerr << "AV_PIX_FMT_YUVA420P16LE: " << AV_PIX_FMT_YUVA420P16LE << '\n';
		std::cerr << "AV_PIX_FMT_YUVA422P16BE: " << AV_PIX_FMT_YUVA422P16BE << '\n';
		std::cerr << "AV_PIX_FMT_YUVA422P16LE: " << AV_PIX_FMT_YUVA422P16LE << '\n';
		std::cerr << "AV_PIX_FMT_YUVA444P16BE: " << AV_PIX_FMT_YUVA444P16BE << '\n';
		std::cerr << "AV_PIX_FMT_YUVA444P16LE: " << AV_PIX_FMT_YUVA444P16LE << '\n';
		std::cerr << "AV_PIX_FMT_VDPAU: " << AV_PIX_FMT_VDPAU << '\n';
		std::cerr << "AV_PIX_FMT_XYZ12LE: " << AV_PIX_FMT_XYZ12LE << '\n';
		std::cerr << "AV_PIX_FMT_XYZ12BE: " << AV_PIX_FMT_XYZ12BE << '\n';
		std::cerr << "AV_PIX_FMT_NV16: " << AV_PIX_FMT_NV16 << '\n';
		std::cerr << "AV_PIX_FMT_NV20LE: " << AV_PIX_FMT_NV20LE << '\n';
		std::cerr << "AV_PIX_FMT_NV20BE: " << AV_PIX_FMT_NV20BE << '\n';
		std::cerr << "AV_PIX_FMT_RGBA64BE: " << AV_PIX_FMT_RGBA64BE << '\n';
		std::cerr << "AV_PIX_FMT_RGBA64LE: " << AV_PIX_FMT_RGBA64LE << '\n';
		std::cerr << "AV_PIX_FMT_BGRA64BE: " << AV_PIX_FMT_BGRA64BE << '\n';
		std::cerr << "AV_PIX_FMT_BGRA64LE: " << AV_PIX_FMT_BGRA64LE << '\n';
		std::cerr << "AV_PIX_FMT_YVYU422: " << AV_PIX_FMT_YVYU422 << '\n';
		std::cerr << "AV_PIX_FMT_YA16BE: " << AV_PIX_FMT_YA16BE << '\n';
		std::cerr << "AV_PIX_FMT_YA16LE: " << AV_PIX_FMT_YA16LE << '\n';
		std::cerr << "AV_PIX_FMT_GBRAP: " << AV_PIX_FMT_GBRAP << '\n';
		std::cerr << "AV_PIX_FMT_GBRAP16BE: " << AV_PIX_FMT_GBRAP16BE << '\n';
		std::cerr << "AV_PIX_FMT_GBRAP16LE: " << AV_PIX_FMT_GBRAP16LE << '\n';
		std::cerr << "AV_PIX_FMT_QSV: " << AV_PIX_FMT_QSV << '\n';
		std::cerr << "AV_PIX_FMT_MMAL: " << AV_PIX_FMT_MMAL << '\n';
		std::cerr << "AV_PIX_FMT_D3D11VA_VLD: " << AV_PIX_FMT_D3D11VA_VLD << '\n';
		std::cerr << "AV_PIX_FMT_CUDA: " << AV_PIX_FMT_CUDA << '\n';
		std::cerr << "AV_PIX_FMT_0RGB: " << AV_PIX_FMT_0RGB << '\n';
		std::cerr << "AV_PIX_FMT_RGB0: " << AV_PIX_FMT_RGB0 << '\n';
		std::cerr << "AV_PIX_FMT_0BGR: " << AV_PIX_FMT_0BGR << '\n';
		std::cerr << "AV_PIX_FMT_BGR0: " << AV_PIX_FMT_BGR0 << '\n';
		std::cerr << "AV_PIX_FMT_YUV420P12BE: " << AV_PIX_FMT_YUV420P12BE << '\n';
		std::cerr << "AV_PIX_FMT_YUV420P12LE: " << AV_PIX_FMT_YUV420P12LE << '\n';
		std::cerr << "AV_PIX_FMT_YUV420P14BE: " << AV_PIX_FMT_YUV420P14BE << '\n';
		std::cerr << "AV_PIX_FMT_YUV420P14LE: " << AV_PIX_FMT_YUV420P14LE << '\n';
		std::cerr << "AV_PIX_FMT_YUV422P12BE: " << AV_PIX_FMT_YUV422P12BE << '\n';
		std::cerr << "AV_PIX_FMT_YUV422P12LE: " << AV_PIX_FMT_YUV422P12LE << '\n';
		std::cerr << "AV_PIX_FMT_YUV422P14BE: " << AV_PIX_FMT_YUV422P14BE << '\n';
		std::cerr << "AV_PIX_FMT_YUV422P14LE: " << AV_PIX_FMT_YUV422P14LE << '\n';
		std::cerr << "AV_PIX_FMT_YUV444P12BE: " << AV_PIX_FMT_YUV444P12BE << '\n';
		std::cerr << "AV_PIX_FMT_YUV444P12LE: " << AV_PIX_FMT_YUV444P12LE << '\n';
		std::cerr << "AV_PIX_FMT_YUV444P14BE: " << AV_PIX_FMT_YUV444P14BE << '\n';
		std::cerr << "AV_PIX_FMT_YUV444P14LE: " << AV_PIX_FMT_YUV444P14LE << '\n';
		std::cerr << "AV_PIX_FMT_GBRP12BE: " << AV_PIX_FMT_GBRP12BE << '\n';
		std::cerr << "AV_PIX_FMT_GBRP12LE: " << AV_PIX_FMT_GBRP12LE << '\n';
		std::cerr << "AV_PIX_FMT_GBRP14BE: " << AV_PIX_FMT_GBRP14BE << '\n';
		std::cerr << "AV_PIX_FMT_GBRP14LE: " << AV_PIX_FMT_GBRP14LE << '\n';
		std::cerr << "AV_PIX_FMT_YUVJ411P: " << AV_PIX_FMT_YUVJ411P << '\n';
		std::cerr << "AV_PIX_FMT_BAYER_BGGR8: " << AV_PIX_FMT_BAYER_BGGR8 << '\n';
		std::cerr << "AV_PIX_FMT_BAYER_RGGB8: " << AV_PIX_FMT_BAYER_RGGB8 << '\n';
		std::cerr << "AV_PIX_FMT_BAYER_GBRG8: " << AV_PIX_FMT_BAYER_GBRG8 << '\n';
		std::cerr << "AV_PIX_FMT_BAYER_GRBG8: " << AV_PIX_FMT_BAYER_GRBG8 << '\n';
		std::cerr << "AV_PIX_FMT_BAYER_BGGR16LE: " << AV_PIX_FMT_BAYER_BGGR16LE << '\n';
		std::cerr << "AV_PIX_FMT_BAYER_BGGR16BE: " << AV_PIX_FMT_BAYER_BGGR16BE << '\n';
		std::cerr << "AV_PIX_FMT_BAYER_RGGB16LE: " << AV_PIX_FMT_BAYER_RGGB16LE << '\n';
		std::cerr << "AV_PIX_FMT_BAYER_RGGB16BE: " << AV_PIX_FMT_BAYER_RGGB16BE << '\n';
		std::cerr << "AV_PIX_FMT_BAYER_GBRG16LE: " << AV_PIX_FMT_BAYER_GBRG16LE << '\n';
		std::cerr << "AV_PIX_FMT_BAYER_GBRG16BE: " << AV_PIX_FMT_BAYER_GBRG16BE << '\n';
		std::cerr << "AV_PIX_FMT_BAYER_GRBG16LE: " << AV_PIX_FMT_BAYER_GRBG16LE << '\n';
		std::cerr << "AV_PIX_FMT_BAYER_GRBG16BE: " << AV_PIX_FMT_BAYER_GRBG16BE << '\n';
		std::cerr << "AV_PIX_FMT_XVMC: " << AV_PIX_FMT_XVMC << '\n';
		std::cerr << "AV_PIX_FMT_YUV440P10LE: " << AV_PIX_FMT_YUV440P10LE << '\n';
		std::cerr << "AV_PIX_FMT_YUV440P10BE: " << AV_PIX_FMT_YUV440P10BE << '\n';
		std::cerr << "AV_PIX_FMT_YUV440P12LE: " << AV_PIX_FMT_YUV440P12LE << '\n';
		std::cerr << "AV_PIX_FMT_YUV440P12BE: " << AV_PIX_FMT_YUV440P12BE << '\n';
		std::cerr << "AV_PIX_FMT_AYUV64LE: " << AV_PIX_FMT_AYUV64LE << '\n';
		std::cerr << "AV_PIX_FMT_AYUV64BE: " << AV_PIX_FMT_AYUV64BE << '\n';
		std::cerr << "AV_PIX_FMT_VIDEOTOOLBOX: " << AV_PIX_FMT_VIDEOTOOLBOX << '\n';
		std::cerr << "AV_PIX_FMT_P010LE: " << AV_PIX_FMT_P010LE << '\n';
		std::cerr << "AV_PIX_FMT_P010BE: " << AV_PIX_FMT_P010BE << '\n';
		std::cerr << "AV_PIX_FMT_GBRAP12BE: " << AV_PIX_FMT_GBRAP12BE << '\n';
		std::cerr << "AV_PIX_FMT_GBRAP12LE: " << AV_PIX_FMT_GBRAP12LE << '\n';
		std::cerr << "AV_PIX_FMT_GBRAP10BE: " << AV_PIX_FMT_GBRAP10BE << '\n';
		std::cerr << "AV_PIX_FMT_GBRAP10LE: " << AV_PIX_FMT_GBRAP10LE << '\n';
		std::cerr << "AV_PIX_FMT_MEDIACODEC: " << AV_PIX_FMT_MEDIACODEC << '\n';
		std::cerr << "AV_PIX_FMT_GRAY12BE: " << AV_PIX_FMT_GRAY12BE << '\n';
		std::cerr << "AV_PIX_FMT_GRAY12LE: " << AV_PIX_FMT_GRAY12LE << '\n';
		std::cerr << "AV_PIX_FMT_GRAY10BE: " << AV_PIX_FMT_GRAY10BE << '\n';
		std::cerr << "AV_PIX_FMT_GRAY10LE: " << AV_PIX_FMT_GRAY10LE << '\n';
		std::cerr << "AV_PIX_FMT_P016LE: " << AV_PIX_FMT_P016LE << '\n';
		std::cerr << "AV_PIX_FMT_P016BE: " << AV_PIX_FMT_P016BE << '\n';
		std::cerr << "AV_PIX_FMT_D3D11: " << AV_PIX_FMT_D3D11 << '\n';
		std::cerr << "AV_PIX_FMT_GRAY9BE: " << AV_PIX_FMT_GRAY9BE << '\n';
		std::cerr << "AV_PIX_FMT_GRAY9LE: " << AV_PIX_FMT_GRAY9LE << '\n';
		std::cerr << "AV_PIX_FMT_GBRPF32BE: " << AV_PIX_FMT_GBRPF32BE << '\n';
		std::cerr << "AV_PIX_FMT_GBRPF32LE: " << AV_PIX_FMT_GBRPF32LE << '\n';
		std::cerr << "AV_PIX_FMT_GBRAPF32BE: " << AV_PIX_FMT_GBRAPF32BE << '\n';
		std::cerr << "AV_PIX_FMT_GBRAPF32LE: " << AV_PIX_FMT_GBRAPF32LE << '\n';
		std::cerr << "AV_PIX_FMT_DRM_PRIME: " << AV_PIX_FMT_DRM_PRIME << '\n';
		// std::cerr << "AV_PIX_FMT_OPENCL: " << AV_PIX_FMT_OPENCL << '\n';
		std::cerr << "AV_PIX_FMT_NB: " << AV_PIX_FMT_NB << '\n' << '\n' << '\n';


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

				std::vector<int> bytes;

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
						}
						if(bytes.empty())
							bytes.resize(frame->width * frame->height * decltype(avg_frame)::channels);
						// bytes.resize(av_image_get_buffer_size(AV_PIX_FMT_RGB0, frame->width, frame->height, 1));

						std::cerr << "Frame #" << best_codec_ctx->frame_number << '\n';
						std::cerr << "\t" << frame->width << 'x' << frame->height << '\n';
						std::cerr << "\t" << bytes.size() << '\n';
						std::cerr << "\t" << frame->format << '\n';
						// std::cerr << "\t-" << frame->crop_top << "; -" << frame->crop_bottom << "; -" << frame->crop_left << "; -" << frame->crop_right << '\n';
						// std::cerr << "\t" << std::hex << static_cast<unsigned int>(frame->data[0][0]) << ' ' << static_cast<unsigned int>(frame->data[0][1]) << ' '
						//           << static_cast<unsigned int>(frame->data[0][2]) << ' ' << static_cast<unsigned int>(frame->data[0][3]) << std::dec << '\n';

						// std::cerr << '\t'
						//          << av_image_copy_to_buffer(bytes.data(), bytes.size(), frame->data, frame->linesize, AV_PIX_FMT_RGB0, frame->width, frame->height, 1)
						//          << '\n';
						// std::reverse(bytes.begin(), bytes.end());

						for(auto x = 0; x < frame->width; ++x)
							for(auto y = 0; y < frame->height; ++y) {
								const auto yuv = &bytes[(y * frame->width + x) * decltype(avg_frame)::channels];
								pick_pixel3(frame, x, y, yuv, yuv + 1, yuv + 2);
								y            = yuv[position.y * size.width + position.x];
								u            = yuv[(position.y / 2) * (size.width / 2) + (position.x / 2) + size.total];
								v            = yuv[(position.y / 2) * (size.width / 2) + (position.x / 2) + size.total + (size.total / 4)];
								const auto b = 1.164 * (yuv[0] - 16) + 2.018 * (yuv[1] - 128);
								const auto g = 1.164 * (yuv[0] - 16) - 0.813 * (yuv[2] - 128) - 0.391 * (yuv[1] - 128);
								const auto r = 1.164 * (yuv[0] - 16) + 1.596 * (yuv[2] - 128);
								yuv[0]       = std::min(std::max(static_cast<int>(r), 0), 255);
								yuv[1]       = std::min(std::max(static_cast<int>(g), 0), 255);
								yuv[2]       = std::min(std::max(static_cast<int>(b), 0), 255);
								// std::cerr << x << 'x' << y << ": " << std::hex << *yuv << ' ' << *(yuv + 1) << ' ' << *(yuv + 2) << std::dec << "=" << *yuv << ' ' << *(yuv +
								// 1) << ' ' << *(yuv + 2) << '\n';
							}

						avg_frame.process_frame(bytes);
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
