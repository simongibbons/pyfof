#! /bin/bash

set -ex

function build_wheels() {
    echo "Building Wheels"

    #docker build . -t pyfof -f Dockerfile-manylinux
    CONTAINER_ID=$(docker create pyfof)
    trap "docker rm -f ${CONTAINER_ID} > /dev/null || true" EXIT INT TERM

    docker start -ai ${CONTAINER_ID}

    mkdir -p wheelhouse
    docker cp ${CONTAINER_ID}:/app/wheelhouse .
}

build_wheels

echo "Built the following wheels"
ls -l ./wheelhouse/pyfof*
