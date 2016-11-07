file_list = \
 src/rollup.cpp
file_list2 = \
 src/export.cpp

noinst_LIBRARIES = librollup.a libexport.a
librollup_a_SOURCES = $(file_list)

libexport_a_SOURCES = $(file_list2)

AM_CXXFLAGS +=    -I. \
                  -Iinclude \
                  -Iinclude/snort \
                  -pg
AM_CXXFLAGS += @AM_CXXFLAGS@
