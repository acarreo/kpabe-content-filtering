FROM debian:buster

ARG COMMIT_RELIC="fe61f2f9518667f5c6d26cf00c3721e98b3f0528"

RUN apt-get update && apt-get install -y --no-install-recommends git \
	cmake make g++ ca-certificates libgmp-dev vim pkg-config && apt-get clean

RUN git clone https://github.com/relic-toolkit/relic /root/relic
WORKDIR /root/relic
RUN git checkout -b current ${COMMIT_RELIC}
COPY compile_relic_curves.sh .
RUN ./compile_relic_curves.sh
WORKDIR /root
