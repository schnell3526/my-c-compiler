NAME=my-c-compiler_env
VERSION=latest

build:
	docker	build	-t	${NAME}:${VERSION}	.

start:
	docker	run	-it	\
	-v	${PWD}/src:/home/user/work	\
	--name	${NAME}	\
	${NAME}:${VERSION}	\
	bash

stop:
	docker	rm	-f	${NAME}

clean:
	docker	rmi	${NAME}:${VERSION}