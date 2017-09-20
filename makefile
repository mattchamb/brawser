CC = emcc
LIBRAW = LibRaw-0.18.4/lib/.libs/libraw.so
LIBJPEG = libjpeg/.libs/libjpeg.so
LENSFUN = lensfun_webassembly/dist/lensfun_wasm.bc

all: brawser.js

brawser.js: $(LIBRAW) $(LIBJPEG) $(LENSFUN) brawser.cpp
	$(CC) $(LIBRAW) $(LIBJPEG) $(LENSFUN) brawser.cpp --bind -O2 -o $@ -s WASM=1 -s BUILD_AS_WORKER=1 -s ALLOW_MEMORY_GROWTH=1

$(LIBRAW):
	cd ./LibRaw-0.18.4 && emmake make

$(LIBJPEG):
	cd ./libjpeg && emmake make

$(LENSFUN):
	cd ./lensfun_webassembly && emmake make