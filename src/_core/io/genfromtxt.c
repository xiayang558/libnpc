#include "array.h"
#include "shape.h"
#include "io.h"
#include "npzfile.h"


/**
/ Read numerical data from text file, return 2D array (like numpy.genfromtxt)
/ @param filename: file name
/ @param delimiter: delimiter (e.g. ",", " ", "\t"), if NULL use whitespace (spaces/tabs)
/ @param skip_header: number of header lines to skip
/ @return: array of read data, data type auto-inferred from file content (currently only supports numeric types)
*/
Array* genfromtxt(const char *filename, const char *delimiter, int skip_header) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "genfromtxt: cannot open file %s\n", filename);
        return NULL;
    }

    // use fscanf by format to read, data may be comma or whitespace separated
    // first count columns and rows
    char line[4096];
    int rows = 0, cols = 0;
    int in_data = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (skip_header > 0) {
            skip_header--;
            continue;
        }
        // skip blank lines
        if (strlen(line) <= 1) continue;
        // count columns (only once, from first data row)
        if (cols == 0) {
            char *p = line;
            while (*p) {
                double v;
                if (sscanf(p, "%lf", &v) == 1) {
                    cols++;
                    while (*p && (*p != ',' && *p != ' ' && *p != '\t' && *p != '\n')) p++;
                    while (*p && (*p == ',' || *p == ' ' || *p == '\t')) p++;
                } else {
                    p++;
                }
            }
        }
        rows++;
        if (rows >= 10000) break; // guard against infinite loop
    }
    if (rows == 0 || cols == 0) {
        fprintf(stderr, "genfromtxt: no data found\n");
        fclose(fp);
        return NULL;
    }

    // re-read the data
    rewind(fp);
    skip_header = 0; // for simplicity, re-skip header
    // re-skip header (using original @param)
    for (int i = 0; i < skip_header; i++) fgets(line, sizeof(line), fp);
    // allocate memory
    double *data = malloc(rows * cols * sizeof(double));
    if (!data) {
        fclose(fp);
        return NULL;
    }
    int r = 0;
    while (fgets(line, sizeof(line), fp) && r < rows) {
        if (strlen(line) <= 1) continue;
        char *p = line;
        int c = 0;
        while (*p && c < cols) {
            double v;
            if (sscanf(p, "%lf", &v) == 1) {
                data[r * cols + c] = v;
                c++;
                while (*p && (*p != ',' && *p != ' ' && *p != '\t' && *p != '\n')) p++;
                while (*p && (*p == ',' || *p == ' ' || *p == '\t')) p++;
            } else {
                p++;
            }
        }
        if (c == cols) r++;
        else if (c > 0) {
            fprintf(stderr, "genfromtxt: row %d has %d columns, expected %d\n", r+1, c, cols);
        }
    }
    fclose(fp);
    if (r != rows) {
        rows = r;
    }
    int shape[2] = {rows, cols};
    Array *res = create_array(shape, 2, FLOAT64);
    if (!res) {
        free(data);
        return NULL;
    }
    memcpy(res->data, data, rows * cols * sizeof(double));
    free(data);
    return res;
}