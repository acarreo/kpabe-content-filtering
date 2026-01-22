.PHONY: local docker

# Default values for variables
RELIC_TAG = 0.6.0
ELLIPTIC_CURVE ?= bls12-381
LSSS_IMAGE_VERSION ?= 0.1.0
ENABLE_COMPRESSION ?= ON

# Path to the Docker directory from abe-lsss repository
DOCKER_DIR = docker

# Target to install dependencies locally
local:
	@echo "Installing dependencies locally..."
	CURVE=$(ELLIPTIC_CURVE) ENABLE_COMPRESSION=$(ENABLE_COMPRESSION) ./install-dependencies.sh || { echo 'Failed to install dependencies'; exit 1; }

# Target to build Docker image
docker:
	@echo "Building Docker image..."
	@if [ -d "$(DOCKER_DIR)" ]; then \
		$(MAKE) -C $(DOCKER_DIR) build_abe_lsss RELIC_TAG=$(RELIC_TAG) CURVE=$(ELLIPTIC_CURVE) ENABLE_COMPRESSION=$(ENABLE_COMPRESSION) ABE_LSSS_VERSION=$(LSSS_IMAGE_VERSION) || { echo 'Failed to build Docker image'; exit 1; } \
	else \
		echo "Docker directory not found"; \
		exit 1; \
	fi
