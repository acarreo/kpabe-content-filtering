.PHONY: local docker

local:
	sudo ./build_on_host.sh

docker:
	tar -czvf lsss.tar.gz --exclude='*.o' --exclude='*.a' ../lsss
	docker build -t presto-content-filtering-use-case:relic-0.6.0--lsss-0.1.1 .
	rm -f lsss.tar.gz