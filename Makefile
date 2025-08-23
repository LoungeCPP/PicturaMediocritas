# The MIT License (MIT)

# Copyright (c) 2017 Lounge<C++>

# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files (the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
# the Software, and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


include configMakefile


LDAR := $(PIC) $(LNCXXAR) $(foreach l,swscale avformat avcodec avutil freeimage $(OS_LD_LIBS),-l$(l))
INCAR := -Iext/mpmc_spmc
VERAR := $(foreach l,PICTURA_MEDIOCRITAS,-D$(l)_VERSION='$($(l)_VERSION)')
TEST_SOURCES := $(sort $(wildcard tests/*.cpp tests/**/*.cpp tests/**/**/*.cpp tests/**/**/**/*.cpp))
BUILD_TEST_SOURCES := $(sort $(wildcard build-tests/*.cpp build-tests/**/*.cpp build-tests/**/**/*.cpp build-tests/**/**/**/*.cpp))
SOURCES := $(sort $(wildcard src/*.cpp src/**/*.cpp src/**/**/*.cpp src/**/**/**/*.cpp))
PREFIX ?= /usr/local

.PHONY : all clean exe tests no-build-tests run-tests

all : exe tests no-build-tests run-tests

install : exe
	mkdir -p $(DESTDIR)/$(PREFIX)/bin $(DESTDIR)/$(PREFIX)/share/man/man1
	cp $(OUTDIR)pictura-mediocritas$(EXE) $(DESTDIR)/$(PREFIX)/bin/
	gzip -9 < pictura-mediocritas.1 > $(DESTDIR)/$(PREFIX)/share/man/man1/pictura-mediocritas.1.gz

clean :
	rm -rf $(OUTDIR)

run-tests : $(OUTDIR)pictura-mediocritas-tests$(EXE)
	$^

exe : $(OUTDIR)pictura-mediocritas$(EXE)
tests : $(OUTDIR)pictura-mediocritas-tests$(EXE)
no-build-tests : $(subst build-tests/,$(BLDDIR)build_test_obj/,$(subst .cpp,.o,$(BUILD_TEST_SOURCES)))


$(OUTDIR)pictura-mediocritas$(EXE) : $(subst $(SRCDIR),$(OBJDIR),$(subst .cpp,.o,$(SOURCES)))
	$(CXX) $(CXXAR) -o$@ $^ $(PIC) $(LDAR)

$(OUTDIR)pictura-mediocritas-tests$(EXE) : $(subst tests/,$(BLDDIR)test_obj/,$(subst .cpp,.o,$(TEST_SOURCES))) $(subst $(SRCDIR),$(OBJDIR),$(subst .cpp,.o,$(filter-out src/main.cpp,$(SOURCES))))
	$(CXX) $(CXXAR) -o$@ $^ $(PIC) $(LDAR)


$(OBJDIR)%.o : $(SRCDIR)%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXAR) $(INCAR) $(VERAR) -c -o$@ $<

$(BLDDIR)test_obj/%.o : tests/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXAR) $(INCAR) -Isrc -c -o$@ $<

$(BLDDIR)build_test_obj/%.o : build-tests/%.cpp
	@mkdir -p $(dir $@)
	! $(CXX) $(CXXAR) $(INCAR) -Isrc -c -o$@ $< 2>$(subst .o,.err_out,$@)
	grep -q "$(shell grep ERROR_MUST_CONTAIN $^ | sed -e 's/#define ERROR_MUST_CONTAIN "//' -e 's/"$$//')" $(subst .o,.err_out,$@)
	touch $@


include $(wildcard $(OBJDIR)*/*.d $(OBJDIR)*.d)
