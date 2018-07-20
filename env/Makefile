default:
	@echo "Try make build, make build-nocache or make push."

build:
	docker build -t simgrid/rsg_ci:latest \
                 -t simgrid/rsg_ci:$$(date --iso-8601) \
                 .

build-nocache:
	docker build -t simgrid/rsg_ci:latest \
                 -t simgrid/rsg_ci:$$(date --iso-8601) \
                 --no-cache \
                 .

push:
	docker push simgrid/rsg_ci
