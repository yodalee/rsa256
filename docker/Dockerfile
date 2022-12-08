FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive
ENV HOME=/workspace/
ENV THE_PREFIX=/usr/
RUN apt update
RUN apt install -y libgtest-dev libgmp-dev build-essential cmake libsystemc-dev \
    python3 wget autoconf perl flex bison zlib1g-dev \
    libgflags-dev libgoogle-glog-dev libprotobuf-dev libboost-all-dev
COPY . .
RUN ./install_verilator.sh