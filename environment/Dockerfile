FROM ubuntu:24.04 as base 
ENV DEBIAN_FRONTEND noninteractive
RUN apt update 
# Install dependencies
RUN apt-get install -y --no-install-recommends git python3 \
    python3-pip openmpi-bin \
    openmpi-common libopenmpi-dev \
    gcc g++ cmake build-essential glibc-source \
    pkgconf autoconf automake libtool libtool-bin make \
    openssh-client openssh-server 
RUN mkdir -p -m 0600 ~/.ssh && \
    ssh-keyscan -H github.com >> ~/.ssh/known_hosts

# Setup SSH
EXPOSE 22
ENV USERNAME=haridev
RUN useradd -rm -d /home/${USERNAME} -s /bin/bash -g root -G sudo -u 2000 ${USERNAME}
RUN  echo '${USERNAME}:${USERNAME}' | chpasswd
RUN echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> \ /etc/sudoers
RUN mkdir /home/${USERNAME}/.ssh
RUN service ssh start
CMD ["/usr/sbin/sshd","-D"]

# Master
FROM base AS master
# Setup spack
ENV SHARED=/mount/shared
ENV SOFTWARE=${SHARED}/software
ENV SPACK_DIR=${SOFTWARE}/spack
ENV PROJECT_DIR ${SHARED}/code
RUN mkdir -p ${SOFTWARE}
RUN mkdir -p ${SPACK_DIR}
RUN git clone https://github.com/spack/spack.git ${SPACK_DIR}
RUN sh ${SPACK_DIR}/share/spack/setup-env.sh
ENV PATH="${PATH}:${SPACK_DIR}/bin"
RUN spack install --sh libuv@1.46.0 ucx@1.15.0
RUN ls /mount
ENTRYPOINT /bin/bash

# worker
FROM base AS worker
RUN ls /mount
ENTRYPOINT /bin/bash
