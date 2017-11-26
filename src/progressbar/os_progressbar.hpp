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


#include <cstdlib>

#ifdef _WIN32
#include <windows.h>

#include <shobjidl.h>

#include <memory>
#else
#endif


namespace pictura_mediocritas {
	/// An OS-dependent progress bar.
	///
	/// Taskbar progress on Windows.
	/// No-op on non-Windows.
	class os_progressbar {
	private:
#ifdef _WIN32
		struct ITaskbarList3_releaser {
			void operator()(ITaskbarList3 * list);
		};

		std::unique_ptr<ITaskbarList3, ITaskbarList3_releaser> taskbar_list;
		HWND console_window;
		std::size_t curr_progress;
		std::size_t max_progress;
#else
#endif

	public:
		/// Create a new progressbar with the specified number of steps.
		///
		/// @param max The number of times the progressbar must be incremented before it is considered complete,
		///            or, in other words, the number of tasks that this progressbar is tracking.
		os_progressbar(std::size_t max);
		~os_progressbar();

		/// Increment progressbar. Don't increment past the initialized # of steps, though.
		void inc();

		/// Set the current status on the progressbar.
		void update(std::size_t value);
	};
}
