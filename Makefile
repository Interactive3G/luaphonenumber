INST_PREFIX = /usr/local/openresty
INST_LIBDIR = $(INST_PREFIX)/lualib

all: luaphonenumber

clean:
	rm -f luaphonenumber.so* luaphonenumber.o

luaphonenumber:
	g++ -I$(INST_PREFIX)/luajit/include/luajit-2.1 -std=c++11 -fPIC -c luaphonenumber.cpp
	g++ -shared -Wl,-soname,luaphonenumber.so.1 -o luaphonenumber.so.1.0 luaphonenumber.o -lphonenumber -lgeocoding
	ln -sf luaphonenumber.so.1.0 luaphonenumber.so
	ln -sf luaphonenumber.so.1.0 luaphonenumber.so.1

install:
	cp luaphonenumber.so $(INST_LIBDIR)
