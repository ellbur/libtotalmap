
# vim: noexpandtab
# vim: ft=make
# vim: shiftwidth=4
# vim: tabstop=4

# https://stackoverflow.com/a/18137056/371739
MKFILE := $(abspath $(lastword $(MAKEFILE_LIST)))
HERE := $(notdir $(patsubst %/,%,$(dir $(MKFILE))))
ROOT = $(abspath $(HERE)/..)

LIB_SOURCES = $(ROOT)/src/*.cpp

SOURCES = $(LIB_SOURCES)

HEADERS = $(ROOT)/include/*.hpp

INCLUDE = -I$(ROOT)/third-party -I$(ROOT)/include

LIB_DIR = $(ROOT)/build-output/lib

LIB = $(LIB_DIR)/libtotalmap.so

$(LIB) : $(SOURCES) $(HEADERS)
	mkdir -p $(LIB_DIR)
	$(CXX) -shared -fPIC -O3 -g -std=c++17 -Wall -Wno-unused-variable -Wno-unused-result $(INCLUDE) $(SOURCES) -o $(LIB)


