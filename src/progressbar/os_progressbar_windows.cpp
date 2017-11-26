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


#ifdef _WIN32


#define INITGUID
#include "os_progressbar.hpp"
#include <limits>


void pictura_mediocritas::os_progressbar::ITaskbarList3_releaser::operator()(ITaskbarList3 * list) {
	list->Release();
}

pictura_mediocritas::os_progressbar::os_progressbar(std::size_t max) : console_window(GetConsoleWindow()), curr_progress(0), max_progress(max) {
	void * tbar;
	CoCreateInstance(CLSID_TaskbarList, nullptr, CLSCTX_ALL, IID_ITaskbarList3, &tbar);
	taskbar_list.reset(static_cast<ITaskbarList3 *>(tbar));

	taskbar_list->SetProgressState(console_window, TBPF_NORMAL);
}

pictura_mediocritas::os_progressbar::~os_progressbar() {
	taskbar_list->SetProgressState(console_window, TBPF_NOPROGRESS);
}

void pictura_mediocritas::os_progressbar::inc() {
	if(curr_progress != std::numeric_limits<std::size_t>::max())
		update(curr_progress + 1);
}

void pictura_mediocritas::os_progressbar::update(std::size_t value) {
	if(value <= max_progress) {
		curr_progress = value;
		taskbar_list->SetProgressValue(console_window, curr_progress, max_progress);
	}
}


#else
#endif
