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


#include <progressbar_cpp/progressbar.hpp>
#include "os_progressbar.hpp"
#include <cstdint>


namespace pictura_mediocritas {
	/// A colluded text and taskbar (on Windows) progress bar.
	class progressbar {
	private:
		progressbar_cpp::progressbar text_bar;
		os_progressbar os_bar;

	public:
		/// Create a new progressbar with the specified label and number of steps.
		///
		/// @param label The label that will prefix the progressbar.
		/// @param max The number of times the progressbar must be incremented before it is considered complete,
		///            or, in other words, the number of tasks that this progressbar is tracking.
		progressbar(const char * label, std::size_t max);
		progressbar(std::string label, std::size_t max);

		/// Increment progressbar. Don't increment past the initialized # of steps, though.
		void inc();

		/// Set the current status on the progressbar.
		void update(std::size_t value);
	};
}
