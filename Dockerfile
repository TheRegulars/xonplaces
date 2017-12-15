FROM alpine:3.7 as builder

RUN apk update && apk add gcc cmake clang make linux-headers musl-dev \
    curl-dev zlib-dev ninja libjpeg-turbo-dev libpng-dev gmp-dev autoconf \
    automake libtool && mkdir /src && mkdir /build

COPY . /src
WORKDIR /build
RUN cd /build && /src/docker_build.sh /src

FROM alpine:3.7
COPY --from=builder /build/darkplaces-dedicated /usr/bin/
RUN apk --no-cache add libjpeg-turbo libjpeg gmp libcurl libpng
