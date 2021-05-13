CFLAGS=-std=c11 -g -static

mcc: util.c mcc.c

test: mcc
	./test.sh

clean:
	@rm -f mcc *.o *~ tmp* a.out

.PHONY: test clean