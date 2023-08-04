export RELIC_TAG="0.6.0"
export CURVE="bls12-381"
export RELIC_INCLUDE="/usr/local/include/relic"
WORKDIR=$(pwd)

sudo apt-get update
sudo apt-get install -y --no-install-recommends cmake make g++ ca-certificates libgmp-dev libmsgpack-dev flex libfl-dev pkg-config

# Build RELIC
git clone https://github.com/relic-toolkit/relic /tmp/relic
cd /tmp/relic
#git switch --detach ${RELIC_TAG}
mkdir "target_${CURVE}"
cd "target_${CURVE}"
../preset/x64-pbc-${CURVE}.sh ..
make -j
sudo make install

# Build LSSS
cd $WORKDIR
make -j -C ../lsss
sudo mv ../lsss/liblsss.a /usr/local/lib/
make -C ../lsss clean
