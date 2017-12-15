#!/bin/sh
set -e

SRC=$1

export CFLAGS="-march=haswell -flto"

cmake -DCOMPILE_CLIENT=OFF \
      -DCOMPILE_MENU=OFF \
      -DCOMPILE_UTILS=OFF \
      -DCOMPILE_VIDEO_CAPTURE=OFF \
      -DCOMPILE_CD=OFF \
      -DSTATIC_LINK_D0=ON \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_C_FLAGS="$CFLAGS" \
    "$SRC"

make -j$(nproc)
