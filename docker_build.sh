#!/bin/sh
set -e

SRC=$1

export CC="/usr/bin/clang"
export CXX="/usr/bin/clang++"
export CFLAGS="-march=haswell"

cmake -DCOMPILE_CLIENT=OFF \
      -DCOMPILE_MENU=OFF \
      -DCOMPILE_UTILS=OFF \
      -DCOMPILE_VIDEO_CAPTURE=OFF \
      -DCOMPILE_CD=OFF \
      -DSTATIC_LINK_D0=ON \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_C_FLAGS="$CFLAGS" \
      -DCMAKE_C_COMPILER_AR="/usr/bin/llvm-ar" \
      -DCMAKE_C_COMPILER_RANLIB="/usr/bin/llvm-ranlib" \
      -DCMAKE_RANLIB="/usr/bin/llvm-ranlib" \
      -DCMAKE_VERBOSE_MAKEFILE=ON \
    "$SRC"

make -j$(nproc)
