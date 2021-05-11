#!/bin/bash

# image=`docker	images	|	awk	'/^<none>/	{	print	$3	}'`
# if	[	${image}	!=	""	]	;	then	\
# 		docker	rmi	${image}	\
# fi

image=`docker images | awk '/^<none>/ {print $3}'`
if [ "$image" != "" ]; then
	docker rmi ${image}
fi

# hoge="A"
# fuga="A"

# if [ $hoge = $fuga ]; then
#     echo "文字列は同じです"
# else
#     echo "文字列は違います"
# fi