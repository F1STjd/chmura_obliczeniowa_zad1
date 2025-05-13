# syntax=docker/dockerfile:1.4

############################
# Stage 1: Build (Multi-arch)
############################
FROM --platform=$BUILDPLATFORM ubuntu:22.04 AS builder

RUN --mount=type=cache,target=/var/lib/apt,id=apt \
    apt-get update && apt-get install -y --no-install-recommends \
        ca-certificates \
        build-essential \
        cmake \
        git \
        libssl-dev \
        libz-dev && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY CMakeLists.txt main.cpp ./

RUN git clone --depth 1 https://github.com/fmtlib/fmt.git && \
    cd fmt && \
    cmake -DCMAKE_BUILD_TYPE=MinSizeRel \
          -DBUILD_SHARED_LIBS=OFF \
          -DCMAKE_INSTALL_PREFIX=/usr . && \
    cmake --build . --target install && \
    cd .. && rm -rf fmt

RUN cmake -S . -B build \
          -DCMAKE_BUILD_TYPE=MinSizeRel \
          -DCMAKE_FIND_LIBRARY_SUFFIXES=".a" \
          -DCMAKE_C_FLAGS="-Oz -ffunction-sections -fdata-sections -fvisibility=hidden" \
          -DCMAKE_CXX_FLAGS="-Oz -ffunction-sections -fdata-sections -fvisibility=hidden -fno-rtti" \
          -DCMAKE_EXE_LINKER_FLAGS="-static-libgcc -static-libstdc++ -Wl,--gc-sections -Wl,--build-id=none -Wl,--strip-all -Wl,--as-needed" && \
    cmake --build build --parallel && \
    strip build/bin/main

############################
# Stage 2: Runtime Image
############################
FROM ubuntu:22.04

RUN apt-get update && apt-get install -y --no-install-recommends \
        libssl3 \
        ca-certificates && \
    rm -rf /var/lib/apt/lists/*

COPY --from=builder /src/build/bin/main /app/weather

EXPOSE 3000
ENTRYPOINT ["/app/weather"]
