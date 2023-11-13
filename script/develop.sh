#!/bin/bash

GIT_ROOT=$(git rev-parse --show-toplevel)

cd $GIT_ROOT && mkdir build

cd build

cmake .. && make
