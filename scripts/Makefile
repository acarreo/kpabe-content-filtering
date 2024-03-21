.PHONY: local docker

local:
	make -C ../abe-lsss

docker:
	docker build --no-cache -t presto-content-filtering-use-case:relic-0.6.0--lsss-1.0 .