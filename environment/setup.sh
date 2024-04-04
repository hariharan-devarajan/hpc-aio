#!/bin/bash

echo Setting ENV Variables
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
export MAIN_PROJECT_DIR=`dirname $SCRIPT_DIR`
echo "MAIN_PROJECT_DIR $MAIN_PROJECT_DIR"
export PROJECT_DIR=/mount/shared/code
export SPACK_DIR=/mount/shared/software/spack
export INSTALL_DIR=/mount/shared/install
export PATH=$PATH:$INSTALL_DIR/bin
export OMPI_ALLOW_RUN_AS_ROOT=1
export OMPI_ALLOW_RUN_AS_ROOT_CONFIRM=1
echo Setting up spack
source ${SPACK_DIR}/share/spack/setup-env.sh

echo Setting up development environment
spack env activate -p ${PROJECT_DIR}/environment

echo Showing current state of packages
spack find