FROM ubuntu:24.04 as build
RUN apt update 
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends git python3 \
    python3-pip openmpi-bin \
    openmpi-common libopenmpi-dev \
    gcc g++ cmake build-essential glibc-source \
    pkgconf autoconf automake libtool libtool-bin make
ENV SPACK_DIR /software/spack
ENV PROJECT_DIR /code
RUN mkdir -p ${SPACK_DIR}
RUN git clone https://github.com/spack/spack.git ${SPACK_DIR}
RUN sh ${SPACK_DIR}/share/spack/setup-env.sh
ENV PATH="${PATH}:${SPACK_DIR}/bin"
RUN spack install --sh libuv@1.46.0 ucx@1.15.0
ENTRYPOINT /bin/bash
