#include "npzfile.h"
#include "array.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int parse_npy_header(const char *header, char **descr, bool *fortran_order, int **shape, int *ndim) {
    const char *p = header;
    p = strstr(p, "'descr'"); if (!p) p = strstr(header, "\"descr\""); if (!p) return -1;
    p = strchr(p, ':'); if (!p) return -1;
    p++; while (*p == ' ' || *p == '\t') p++;
    if (*p == '\'' || *p == '"') p++;
    const char *q = p; while (*q && *q != '\'' && *q != '"') q++;
    *descr = malloc(q - p + 1); strncpy(*descr, p, q - p); (*descr)[q - p] = '\0';
    p = strstr(header, "fortran_order"); if (!p) return -1;
    p = strchr(p, ':'); if (!p) return -1;
    p++; while (*p == ' ' || *p == '\t') p++;
    if (strncmp(p, "True", 4) == 0) *fortran_order = true; else *fortran_order = false;
    p = strstr(header, "'shape'"); if (!p) p = strstr(header, "\"shape\""); if (!p) return -1;
    p = strchr(p, ':'); if (!p) return -1;
    p++; while (*p == ' ' || *p == '\t') p++;
    while (*p == '(' || *p == ' ') p++;
    const char *end = strchr(p, ')'); if (!end) return -1;
    char shape_buf[256]; int len = end - p; if (len >= 256) len = 255;
    strncpy(shape_buf, p, len); shape_buf[len] = '\0';
    int dims[32]; int nd = 0;
    char *saveptr; char *token = strtok_r(shape_buf, ",", &saveptr);
    while (token && nd < 32) { while (*token == ' ') token++; dims[nd++] = atoi(token); token = strtok_r(NULL, ",", &saveptr); }
    *ndim = nd;
    if (nd > 0) { *shape = malloc(nd * sizeof(int)); memcpy(*shape, dims, nd * sizeof(int)); }
    else *shape = NULL;
    return 0;
}
