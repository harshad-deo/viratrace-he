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