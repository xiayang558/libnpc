#include <stdio.h>

static int _buffer_size = 8192;  // defaultbufferlargesmall

int getbufsize(void) {
    return _buffer_size;
}

void setbufsize(int size) {
    if (size > 0) {
        _buffer_size = size;
    } else {
        fprintf(stderr, "setbufsize: size must be positive\n");
    }
}