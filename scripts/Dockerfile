FROM debian:bullseye

ARG RELIC_TAG="0.6.0"

ENV CURVE=${CURVE:-bls12-381}
ENV RELIC_INCLUDE="/usr/local/include/relic"

RUN apt-get update && apt-get install -y --no-install-recommends git \
	cmake make g++ ca-certificates libgmp-dev vim flex libfl-dev pkg-config && apt-get clean

RUN git clone --branch ${RELIC_TAG} --depth 1 https://github.com/relic-toolkit/relic /tmp/relic

RUN mkdir -p /tmp/relic/build-${CURVE}
WORKDIR /tmp/relic/build-${CURVE}
RUN ../preset/x64-pbc-${CURVE}.sh ..
RUN make -j && make install

WORKDIR /tmp
COPY lsss.tar.gz .
RUN tar -xzvf lsss.tar.gz && rm -f lsss.tar.gz
RUN make -C lsss && make -C lsss install

WORKDIR /root
