#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xgbitmap.h"
#include <time.h>

int main(int argc, char *argv[]) {
	Display *display;
	Window window;
	XEvent event;
	GC gc;
	XSizeHints sizehints;
	Pixmap bitmap;
	int screen, depth, frame, direction, exposed, x11_fd, running;
	fd_set in_fds;
	struct timeval tv;
	static char *title = "XGoldie";
	display = XOpenDisplay(NULL);
	if (display == NULL) {
		fprintf(stderr, "Cannot open display\n");
		exit(1);
	}
	screen = DefaultScreen(display);
	depth = DefaultDepth(display, screen);
	window = XCreateSimpleWindow(
	        display,
	        DefaultRootWindow(display),
	        0, 0,
	        xgwidth, xgheight,
	        5,
	        BlackPixel(display, screen), WhitePixel(display, screen)
	    );
	sizehints.flags = PMinSize | PMaxSize;
	sizehints.min_width = xgwidth;
	sizehints.max_width = xgwidth;
	sizehints.min_height = xgheight;
	sizehints.max_height = xgheight;
	XSetWMNormalHints(display, window, &sizehints);
	XStoreName(display, window, title);
	Atom wmDeleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(display, window, &wmDeleteMessage, 1);
	XSelectInput(display, window, ExposureMask | KeyPressMask);
	XMapWindow(display, window);
	frame = 0;
	direction = 0;
	exposed = 0;
	x11_fd = ConnectionNumber(display);
	// Main loop
	while(1) {
		// Create a File Description Set containing x11_fd
		FD_ZERO(&in_fds);
		FD_SET(x11_fd, &in_fds);
		// Set our timer.
		tv.tv_usec = 50000;
		tv.tv_sec = 0;
		// Wait for X Event or a Timer
		int num_ready_fds = select(x11_fd + 1, &in_fds, NULL, NULL, &tv);
		if (num_ready_fds > 0) {
			//Event Received
			if (event.type == Expose) {
				//Should I do something here?
			}
			if (event.type == ClientMessage) {
				if (event.xclient.data.l[0] == wmDeleteMessage)
				                    break;
			}
			if (event.type == KeyPress) {
				if ( event.xkey.keycode == 0x09 )
				                    break;
			}
		} else if (num_ready_fds == 0) {
			//Timer Fired
			if (direction == 0) {
				if (++frame >= 10) direction = 1;
			} else {
				if (--frame <= 0) direction = 0;
			}
			bitmap = XCreatePixmapFromBitmapData(display, window, xgbits+(frame*3616), xgwidth, xgheight, WhitePixel(display, screen), BlackPixel(display, screen), depth);
			XCopyArea(display, bitmap, window, DefaultGC(display, screen), 0, 0, xgwidth, xgheight, 0, 0);
		} else {
			printf("An error occured!\n");
		}
		// Handle XEvents and flush the input 
		while(XPending(display))
		            XNextEvent(display, &event);
	}
	XCloseDisplay(display);
	return 0;
}
