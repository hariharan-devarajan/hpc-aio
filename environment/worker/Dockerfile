FROM ubuntu:24.04 as build
RUN apt update 
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends git python3 \
    python3-pip openmpi-bin \
    openmpi-common libopenmpi-dev \
    gcc g++ cmake build-essential glibc-source \
    pkgconf autoconf automake libtool libtool-bin make \
    openssh-client  openssh-server
RUN mkdir -p -m 0600 ~/.ssh && \
    ssh-keyscan -H github.com >> ~/.ssh/known_hosts
RUN service ssh restart
ENV SHARED=/mount/shared
ENV SOFTWARE=${SHARED}/software
ENV SPACK_DIR=${SOFTWARE}/spack
ENV PROJECT_DIR ${SHARED}/code
RUN sh ${SPACK_DIR}/share/spack/setup-env.sh
ENV PATH="${PATH}:${SPACK_DIR}/bin"
EXPOSE 22
ENTRYPOINT /bin/bash
