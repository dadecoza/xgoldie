#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/select.h>
#include "xgbitmap.h"

int main(int argc, char *argv[]) {
    Display *display;
    Window window;
    XEvent event;
    GC gc;
    XSizeHints sizehints;
    Pixmap bitmaps[11];
    int screen, depth, frame, direction, exposed, running, x11_fd, i;
    fd_set in_fds;
    static char *title = "XGoldie";
    struct timeval tv;

    display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "Cannot open display\n");
        exit(1);
    }
    screen = DefaultScreen(display);
    depth = DefaultDepth(display, screen);
    gc = DefaultGC(display, screen);
    window = XCreateSimpleWindow(
        display,
        DefaultRootWindow(display),
        0, 0,
        xgwidth, xgheight,
        5,
        WhitePixel(display, screen), BlackPixel(display, screen)
    );
    for (i = 0; i < 11; i++) {
        char *ptr = xgbits+(i*3616);
        bitmaps[i] = XCreatePixmapFromBitmapData(
            display,
            window,
            ptr,
            xgwidth, xgheight,
            WhitePixel(display, screen), BlackPixel(display, screen),
            depth
        );
    }
    sizehints.flags = PMinSize | PMaxSize;
    sizehints.min_width = 0;
    sizehints.max_width = xgwidth;
    sizehints.min_height = 0;
    sizehints.max_height = xgheight;
    XSetWMNormalHints(display, window, &sizehints);
    XStoreName(display, window, title);
    Atom wmDeleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", 0);
    XSetWMProtocols(display, window, &wmDeleteMessage, 1);
    XSelectInput(display, window, ExposureMask | KeyPressMask);
    XSetGraphicsExposures(display, gc, 0);
    XMapWindow(display, window);
    XFlush(display);
    x11_fd = ConnectionNumber(display);
    frame = 0;
    direction = 0;
    exposed = 0;
    running = 1;

    // Main loop
    while(running) {
        FD_ZERO(&in_fds);
        FD_SET(x11_fd, &in_fds);

        // Time between frames
        tv.tv_usec = 50000;
        tv.tv_sec = 0;

        int num_ready_fds = select(x11_fd + 1, &in_fds, NULL, NULL, &tv);
        if (num_ready_fds > 0) {
            // Event Received!
        } else if (num_ready_fds == 0) {
            // Time to animate!
            if (exposed) {
                if (direction == 0) {
                    if (++frame >= 10) direction = 1;
                } else {
                    if (--frame <= 0) direction = 0;
                }
                XCopyArea(display, bitmaps[frame], window, gc, 0, 0, xgwidth, xgheight, 0, 0);
            }
        } else {
            printf("An error occured!\n");
        }

        while(XPending(display)) {
            XNextEvent(display, &event);
            if (event.type == Expose) {
                exposed = 1;
            }

            if (event.type == ClientMessage) {
                if (event.xclient.data.l[0] == wmDeleteMessage) running = 0;
            }

            if (event.type == KeyPress) {
                if (( event.xkey.keycode == 0x09 )||( event.xkey.keycode == 0x3D )) running = 0;
            }
        }
    }
    XCloseDisplay(display);
    return 0;
}
