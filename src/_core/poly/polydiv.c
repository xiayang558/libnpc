#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "array.h"
#include "utils.h"

/* strip leading zeros helper */
static int lead_zero(double *d, int n) { int i=0; while(i<n-1 && fabs(d[i])<1e-15)i++; return i; }

Array* polydiv(Array *u, Array *v, Array **r_out) {
    *r_out = NULL;
    if (!u || !v || u->ndim != 1 || v->ndim != 1) { fprintf(stderr,"polydiv: 1D arrays required\n"); return NULL; }
    int nu = u->size, nv = v->size;
    double *ud = (double*)u->data, *vd = (double*)v->data;
    /* strip leading zeros from divisor */
    int v0 = lead_zero(vd, nv); nv -= v0;
    if (nv == 0) { fprintf(stderr,"polydiv: zero divisor\n"); return NULL; }
    double vlead = vd[v0];
    int nq = nu - nv + 1;
    if (nq <= 0) {
        /* degree(u) < degree(v): quotient = [0], remainder = u */
        Array *q = create_array((int[]){1}, 1, FLOAT64);
        if (q) { double *qd=(double*)q->data; qd[0]=0; }
        int ru_start = lead_zero(ud, nu);
        int rn = nu - ru_start;
        if (rn == 0) rn = 1;
        Array *rem = create_array((int[]){rn}, 1, FLOAT64);
        if (rem) { double *rd=(double*)rem->data; for(int i=0;i<rn;i++) rd[i]=ud[ru_start+i]; }
        *r_out = rem;
        return q;
    }
    /* Work copy of dividend */
    double *w = malloc(nu * sizeof(double));
    memcpy(w, ud, nu * sizeof(double));
    double *q = calloc(nq, sizeof(double));

    for (int i = 0; i < nq; i++) {
        q[i] = w[i] / vlead;
        for (int j = 0; j < nv; j++) w[i + j] -= q[i] * vd[v0 + j];
    }
    /* remainder starts at nq, strip leading zeros */
    int r_start = lead_zero(w + nq, nu - nq);
    int rn = nu - nq - r_start;
    if (rn <= 0) rn = 1;

    Array *quot = create_array((int[]){nq}, 1, FLOAT64);
    if (quot) memcpy(quot->data, q, nq * sizeof(double));
    Array *rem = create_array((int[]){rn}, 1, FLOAT64);
    if (rem) {
        double *rd = (double*)rem->data;
        if (rn == 1 && r_start >= nu - nq) rd[0] = 0;
        else for (int i = 0; i < rn; i++) rd[i] = w[nq + r_start + i];
    }
    *r_out = rem;
    free(w); free(q);
    return quot;
}
