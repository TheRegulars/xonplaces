#!/bin/sh
set -e

SRC=$1

export CFLAGS="-march=haswell"

cmake -DCOMPILE_CLIENT=OFF \
      -DCOMPILE_MENU=OFF \
      -DCOMPILE_UTILS=OFF \
      -DCOMPILE_VIDEO_CAPTURE=OFF \
      -DCOMPILE_CD=OFF \
      -DBUILD_TESTING=OFF \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_C_FLAGS="$CFLAGS" \
      -DCMAKE_CXX_FLAGS="$CFLAGS" \
      -DCMAKE_VERBOSE_MAKEFILE=ON \
      "$SRC"

make -j$(nproc)
