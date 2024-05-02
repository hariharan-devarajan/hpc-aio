#!/bin/bash

echo Setting ENV Variables
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
export MAIN_PROJECT_DIR=`dirname $SCRIPT_DIR`
echo "MAIN_PROJECT_DIR $MAIN_PROJECT_DIR"
export PROJECT_DIR=/mount/shared/code
export INSTALL_DIR=/mount/shared/install
export SPACK_DIR=/mount/shared/software/spack
export PATH=$PATH:$INSTALL_DIR/bin

echo Setting up Project
rm -rf ${PROJECT_DIR}
cp -r ${MAIN_PROJECT_DIR} ${PROJECT_DIR}

echo Setting up spack
source ${SPACK_DIR}/share/spack/setup-env.sh

echo Setting up development environment
spack env activate -p ${PROJECT_DIR}/environment
spack install

python3 -m venv /mount/shared/pip-env
source /mount/shared/pip-env/bin/activate
pip install dlio-profiler-py