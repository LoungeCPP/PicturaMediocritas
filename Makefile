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


LDAR := $(PIC) $(LNCXXAR) $(foreach l, ,-L$(OUTDIR)$(l)) $(foreach l, ,-l$(l))
VERAR := $(foreach l,CATCH2 PICTURA_MEDIOCRITAS TCLAP,-D$(l)_VERSION='$($(l)_VERSION)')
INCAR := $(foreach l,$(foreach l,$(foreach l,TCLAP,$(l)/include) Catch2/single_include,ext/$(l)),-isystem$(l))
TEST_SOURCES := $(sort $(wildcard tests/*.cpp tests/**/*.cpp tests/**/**/*.cpp tests/**/**/**/*.cpp))
BUILD_TEST_SOURCES := $(sort $(wildcard build-tests/*.cpp build-tests/**/*.cpp build-tests/**/**/*.cpp build-tests/**/**/**/*.cpp))
SOURCES := $(sort $(wildcard src/*.cpp src/**/*.cpp src/**/**/*.cpp src/**/**/**/*.cpp))

.PHONY : all clean cpr exe tests no-build-tests run-tests

all : cpr exe tests no-build-tests run-tests

clean :
	rm -rf $(OUTDIR)

run-tests : $(OUTDIR)pictura-mediocritas-tests$(EXE)
	$^

exe : $(OUTDIR)pictura-mediocritas$(EXE)
tests : $(OUTDIR)pictura-mediocritas-tests$(EXE)
no-build-tests : $(subst build-tests/,$(BLDDIR)build_test_obj/,$(subst .cpp,$(OBJ),$(BUILD_TEST_SOURCES)))


$(OUTDIR)pictura-mediocritas$(EXE) : $(subst $(SRCDIR),$(OBJDIR),$(subst .cpp,$(OBJ),$(SOURCES)))
	$(CXX) $(CXXAR) -o$@ $^ $(PIC) $(LDAR)

$(OUTDIR)pictura-mediocritas-tests$(EXE) : $(subst tests/,$(BLDDIR)test_obj/,$(subst .cpp,$(OBJ),$(TEST_SOURCES))) $(subst $(SRCDIR),$(OBJDIR),$(subst .cpp,$(OBJ),$(filter-out src/main.cpp,$(SOURCES))))
	$(CXX) $(CXXAR) -o$@ $^ $(PIC) $(LDAR)


$(OBJDIR)%$(OBJ) : $(SRCDIR)%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXAR) $(INCAR) $(VERAR) -c -o$@ $^

$(BLDDIR)test_obj/%$(OBJ) : tests/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXAR) $(INCAR) -Isrc -c -o$@ $^

$(BLDDIR)build_test_obj/%$(OBJ) : build-tests/%.cpp
	@mkdir -p $(dir $@)
	! $(CXX) $(CXXAR) $(INCAR) -Isrc -c -o$@ $^ 2>$(subst $(OBJ),.err_out,$@)
	grep -q "$(shell grep ERROR_MUST_CONTAIN $^ | sed -e 's/#define ERROR_MUST_CONTAIN "//' -e 's/"$$//')" $(subst $(OBJ),.err_out,$@)
	touch $@
