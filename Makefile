CC=gcc
CFLAGS=-I.
DEPS = xgbitmap.h

xgoldie: xgoldie.c 
	$(CC) -o xgoldie xgoldie.c -lX11

clean:
	rm -f xgoldie

