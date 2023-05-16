FROM debian:bullseye

ARG RELIC_TAG="0.6.0"

RUN apt-get update && apt-get install -y --no-install-recommends git \
	cmake make g++ ca-certificates libgmp-dev vim flex libfl-dev pkg-config && apt-get clean

RUN git clone --branch ${RELIC_TAG} --depth 1 https://github.com/relic-toolkit/relic /root/relic
WORKDIR /root/relic
COPY compile_relic_curves.sh .
RUN ./compile_relic_curves.sh

WORKDIR /root
COPY lsss.tar.gz .
COPY compile_lsss.sh .
RUN ./compile_lsss.sh
