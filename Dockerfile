FROM alpine:3.7 as builder

RUN apk update && apk add gcc cmake clang make linux-headers musl-dev ninja \
    curl-dev zlib-dev libjpeg-turbo-dev libpng-dev gmp-dev autoconf clang-dev \
    automake libtool binutils binutils-gold llvm5-static llvm5 llvm5-dev \
    freetype-dev && mkdir /src && mkdir /build

RUN cp /usr/lib/llvm5/lib/LLVMgold.so /usr/lib/
COPY . /src
WORKDIR /build
RUN cd /build && /src/docker_build.sh /src

FROM alpine:3.7
COPY --from=builder /build/darkplaces-dedicated /usr/bin/
RUN apk --no-cache add libjpeg-turbo freetype libjpeg gmp libcurl libpng
