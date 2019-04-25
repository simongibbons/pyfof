#! /bin/bash

function build_wheels() {
    echo "Building Wheels for $1"

    docker build . -t pyfof -f Dockerfile-$1
    CONTAINER_ID=$(docker create pyfof)
    trap "docker rm -f ${CONTAINER_ID} > /dev/null || true" EXIT INT TERM

    docker start -ai ${CONTAINER_ID}

    docker cp ${CONTAINER_ID}:/app/dist/ .
}

build_wheels py27
build_wheels py36
build_wheels py37

echo "Built the following wheels"
ls -l ./dist/*
