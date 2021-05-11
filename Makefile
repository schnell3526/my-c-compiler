CONTAINER_NAME=my-c-compiler_env
VERSION=latest
SHELL=/bin/bash
DOCKERFILE_DIR=docker
DOCKERFILE_NAME=Dockerfile

br: ## build&run
	@make b
	@make r

b: ## build Dockerfile
	docker build --file $(DOCKERFILE_DIR)/$(DOCKERFILE_NAME) -t ${CONTAINER_NAME}:${VERSION} .

r: ## run container
	docker run -it \
	-v ${PWD}/src:/home/user/work \
	${CONTAINER_NAME}:${VERSION} \
	/bin/bash


NONE_DOCKER_IMAGES=`docker images -f dangling=true -q`
EXITED_DOCKER_CONTAINER=`docker ps -aq`

clean: ## clean images & containers
	-@make clean-images
	-@make clean-containers

clean-images:
	docker rmi -f $(NONE_DOCKER_IMAGES)

clean-containers:
	docker rm -f $(EXITED_DOCKER_CONTAINER)

help: ## this help
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-20s\033[0m %s\n", $$1, $$2}'