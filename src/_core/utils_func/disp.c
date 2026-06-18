#include <stdio.h>
#include "array.h"
void disp(const char* mesg, int add_newline) {
    if (mesg == NULL) {
        fprintf(stderr, "disp: mesg is NULL\n");
        return;
    }
    printf("%s", mesg);
    if (add_newline) {
        printf("\n");
    }
    fflush(stdout);
}


// helper: insert new axis at given position (returns view, shares data)
