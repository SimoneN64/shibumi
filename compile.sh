#!/bin/bash

cmake -DCMAKE_BUILD_TYPE=$1 -B build-$1
make -j8 -C build-$1