#!/bin/sh

make -f ./makefiles/complicated-example-layout.make
make -f ./makefiles/simple-example-layout.make
make -f ./makefiles/basic-sending-example.make
make -f ./makefiles/basic-receiving-example.make
make -f ./makefiles/print-mappings.make
make -f ./makefiles/show-transformed-keys.make

