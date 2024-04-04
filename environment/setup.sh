#!/bin/bash

echo Setting ENV Variables
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
export MAIN_PROJECT_DIR=`dirname $SCRIPT_DIR`
export PROJECT_DIR=/code
export INSTALL_DIR=/install
export PATH=$PATH:$INSTALL_DIR/bin

echo Setting up Project
rm ${PROJECT_DIR}
ln -s ${MAIN_PROJECT_DIR} ${PROJECT_DIR}

echo Setting up spack
source ${SPACK_DIR}/share/spack/setup-env.sh

echo Setting up development environment
spack env activate -p ${PROJECT_DIR}/environment
spack concretize
while true; do
    read -p "Do you wish to install this program? " yn
    case $yn in
        [Yy]* ) break;;
        [Nn]* ) exit;;
        * ) echo "Please answer yes or no.";;
    esac
done
spack install