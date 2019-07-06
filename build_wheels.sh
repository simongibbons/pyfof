#! /bin/bash

set -ex

# Compile wheels
for PYBIN in /opt/python/*/bin; do
    "${PYBIN}/pip" install -r /app/requirements.txt
    "${PYBIN}/pip" wheel /app/ -w wheelhouse/
done

for WHL in wheelhouse/pyfof*.whl; do
    auditwheel repair "${WHL}"
done

