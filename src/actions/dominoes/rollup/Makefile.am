file_list = \
 src/rollup.cpp

noinst_LIBRARIES = librollup.a
librollup_a_SOURCES = $(file_list)

AM_CXXFLAGS +=    -I. \
                  -Iinclude \
                  -Iinclude/snort \
                  -pg
AM_CXXFLAGS += @AM_CXXFLAGS@
