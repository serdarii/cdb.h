
CC=gcc
LINKER=ld

CFLAGS=

.PHONY: main run

main:
	${CC} ${CFLAGS} $@.c -o build/$@

debug:
	${CC} ${CFLAGS} -g main.c -o build/$@
	gdb ./build/debug

run: main
	./build/$<

clean:
	rm -rf ./build/*
