FROM ubuntu:24.04 as build
RUN apt update 
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends git python3 \
    python3-pip openmpi-bin \
    openmpi-common libopenmpi-dev \
    gcc g++ cmake pkgconf 
ENV SPACK_DIR /software/spack
ENV PROJECT_DIR /code
RUN ln -s /com.docker.devenvironments.code ${PROJECT_DIR}
RUN mkdir -p ${SPACK_DIR}
RUN git clone https://github.com/spack/spack.git ${SPACK_DIR}
RUN ${SPACK_DIR}/share/spack/setup-env.sh
ENV PATH="${PATH}:${SPACK_DIR}/bin"
RUN spack env activate -p ${PROJECT_DIR}/environment
RUN spack install
ENTRYPOINT /bin/bash
