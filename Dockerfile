FROM alpine:3.11.6

# setting up build dependencies
RUN apk update \
  && apk add \
  build-base \
  cmake \
  git \
  zlib

WORKDIR /opt

# installing gsl
RUN git clone https://github.com/microsoft/GSL.git \
  && cd GSL \
  && git checkout v3.0.1 \
  && mkdir build \
  && cd build \
  && cmake .. \
  && make \
  && make install

# installing seal
RUN git clone https://github.com/microsoft/SEAL.git \
  && cd SEAL \
  && git checkout v3.5.1 \
  && mkdir build \
  && cd build \
  && cmake .. \
  && make \
  && make install

# installing google test
RUN git clone https://github.com/google/googletest.git \
  && cd googletest \
  && git checkout release-1.10.0 \
  && mkdir build \
  && cd build \
  && cmake .. \
  && make \
  && make install

# installing google benchmark
RUN git clone https://github.com/google/benchmark.git \
  && cd benchmark \
  && git checkout v1.5.0 \
  && mkdir build \
  && cd build \
  && cmake -DCMAKE_BUILD_TYPE=Release -DBENCHMARK_ENABLE_LTO=true -DBENCHMARK_ENABLE_GTEST_TESTS=false -DBENCHMARK_ENABLE_TESTING=false .. \
  && make \
  && make install
