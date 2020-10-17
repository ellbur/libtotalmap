
# vim: noexpandtab
# vim: ft=make
# vim: shiftwidth=4
# vim: tabstop=4

# https://stackoverflow.com/a/18137056/371739
MKFILE := $(abspath $(lastword $(MAKEFILE_LIST)))
HERE := $(notdir $(patsubst %/,%,$(dir $(MKFILE))))
ROOT = $(abspath $(HERE)/..)

LIB_SOURCES = 

TEST_SOURCES = $(ROOT)/examples/basic-dev-input-example/*.cpp

SOURCES = $(LIB_SOURCES) $(TEST_SOURCES)

HEADERS = 

INCLUDE = 

BIN_DIR = $(ROOT)/build-output/bin
BIN = $(BIN_DIR)/basic-dev-input-example

$(BIN) : $(SOURCES) $(HEADERS)
	mkdir -p $(BIN_DIR)
	$(CXX) -O3 -g -std=c++17 -Wall -Wno-unused-variable -Wno-unused-result $(INCLUDE) $(SOURCES) -lboost_program_options -o $(BIN)

