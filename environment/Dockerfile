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
RUN spack env create -d ${PROJECT_DIR}/environment
ENTRYPOINT /bin/bash
