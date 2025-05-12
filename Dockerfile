# ----------------
# Stage 1: build
# ----------------
  FROM alpine:3.21 AS builder

  # zaleznosci
  RUN apk add --no-cache \
        build-base \
        cmake \
        git \
        linux-headers \
        musl-dev \
        openssl-libs-static \
        libstdc++-dev 
  
  WORKDIR /src
  
  # kopiowanie kodu i cmake
  COPY CMakeLists.txt main.cpp ./

  # 1) Build static fmt
  RUN git clone --depth 1 https://github.com/fmtlib/fmt.git \
  && mkdir fmt/build && cd fmt/build \
  && cmake -DCMAKE_BUILD_TYPE=MinSizeRel \
           -DBUILD_SHARED_LIBS=OFF \
           -DCMAKE_INSTALL_PREFIX=/usr \
           .. \
  && make -j$(nproc) install
  
  # 2) httplib (header-only)
  RUN git clone --depth 1 https://github.com/yhirose/cpp-httplib.git \
    && cp -R cpp-httplib /usr/include/httplib
  
  # Build pod najmniejszy rozmiar, statycznie linkowany
  WORKDIR /src
  RUN mkdir build && cd build \
    && cmake \
         -DCMAKE_BUILD_TYPE=MinSizeRel \
         -DBUILD_SHARED_LIBS=OFF \
         -DCMAKE_FIND_LIBRARY_SUFFIXES=".a" \
         -DCMAKE_C_FLAGS="-Os -ffunction-sections -fdata-sections -fvisibility=hidden -flto" \
         -DCMAKE_CXX_FLAGS="-Os -ffunction-sections -fdata-sections -fvisibility=hidden -flto -fno-rtti" \
         -DCMAKE_EXE_LINKER_FLAGS="-static-libgcc -static-libstdc++ \
         -Wl,--gc-sections -s" \
         .. \
    && make -j$(nproc)
  
  RUN strip build/bin/main
  
  # --------------------------------
  # Stage 2: minimal obraz
  # --------------------------------
  FROM scratch
  
  # kopiowanie pliku wykonywalnego
  COPY --from=builder /src/build/bin/main /app/weather
  
  EXPOSE 3000
  ENTRYPOINT ["/app/weather"]
  