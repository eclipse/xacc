FROM alpine:latest

# builds an image of 620 MB

COPY patch_Error.cpp /tmp/
COPY patch_glucose.hpp /tmp/

RUN apk add --no-cache build-base gcc binutils cmake blas-dev lapack-dev python3-dev \
    curl-dev git linux-headers py3-pip py3-scipy gfortran \
    && python3 -m pip install --upgrade pip \
    && git clone --recursive https://github.com/eclipse/xacc \
    && mv /tmp/patch_Error.cpp xacc/tpls/cppmicroservices/util/src/Error.cpp \
    && mv /tmp/patch_glucose.hpp xacc/tpls/staq/libs/glucose/glucose.hpp \
    && cd xacc && mkdir build && cd build/ \
    && cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local/aideqc/qcor && make -j8 install \
    && python3 -m pip install ipopo --user \
    && cd ../../ && rm -rf /xacc

ENV PYTHONPATH "${PYTHONPATH}:/usr/local/aideqc/qcor"
