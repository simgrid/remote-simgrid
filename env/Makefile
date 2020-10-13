default:
	@echo "Try make build, make build-nocache or make push."

build:
	docker build -t simgrid/ci-empty-nixos:latest \
                 -t simgrid/ci-empty-nixos:$$(date --iso-8601) \
                 .

build-nocache:
	docker build -t simgrid/ci-empty-nixos:latest \
                 -t simgrid/ci-empty-nixos:$$(date --iso-8601) \
                 --no-cache \
                 .

push:
	docker push simgrid/ci-empty-nixos
