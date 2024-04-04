FROM ubuntu:24.04 as build
RUN apt update 
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends git python3 \
    python3-pip openmpi-bin \
    openmpi-common libopenmpi-dev \
    gcc g++ cmake build-essential glibc-source \
    pkgconf autoconf automake libtool libtool-bin make \
    openssh-client
RUN mkdir -p -m 0600 ~/.ssh && \
    ssh-keyscan -H github.com >> ~/.ssh/known_hosts
ENV SHARED=/mount/shared
ENV SOFTWARE=${SHARED}/software
ENV SPACK_DIR=${SOFTWARE}/spack
ENV PROJECT_DIR ${SHARED}/code
ENTRYPOINT /bin/bash
