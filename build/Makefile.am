AUTOMAKE_OPTIONS = foreign
bin_PROGRAMS=storm stormcompiler stormrun

#defined in build/configure.ac
if DEBUG
AM_CXXFLAGS = -g3 -O0
else
## -O3 for minmal execution time
AM_CXXFLAGS = -O3
endif

storm_CXXFLAGS = $(AM_CXXFLAGS)
storm_SOURCES = \
	../Storm/storm.cc

stormcompiler_CXXFLAGS = $(AM_CXXFLAGS)
stormcompiler_SOURCES = \
	../parser/parser.cc \
	../parser/svalues.cc \
	../parser/lexer.cc \
	../parser/functions.cc \
	../parser/storm_calls.cc \
	../Storm/util.cc \
	../file/read.cc

stormrun_CFLAGS = $(AM_CXXFLAGS)
stormrun_SOURCES = \
	../interpreter/interpreter.cc \
	../interpreter/mem.cc \
	../Storm/util.cc \
	../file/read.cc