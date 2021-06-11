.PHONY: all test clean

all: bfptr.so

LUA_BIN = /usr/local/bin/lua

CFLAGS += -g3 -O2 -std=c99 -rdynamic -Wall -fPIC -shared

bfptr.so: bfptr.c
	gcc $(CFLAGS) -o $@ $^

clean:
	rm bfptr.so

test:
	$(LUA_BIN) test.lua
