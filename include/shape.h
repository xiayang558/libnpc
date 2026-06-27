#include "def.h"
#include "array.h"

#ifdef __cplusplus
extern "C" {
#endif

Array* atleast_1d(Array *a);
Array* atleast_2d(Array *a);
Array* atleast_3d(Array *a);

Array* block(Array **blocks, int rows, int cols);

/* ------------------------------- split function family ------------------------------- */
Array** split(Array *ary, Array *indices_or_sections, int axis, int *num_splits);
Array** dsplit(Array *ary, Array *indices_or_sections, int *num_splits);
Array** hsplit(Array *ary, Array *indices_or_sections, int *num_splits);
Array** vsplit(Array *ary, Array *indices_or_sections, int *num_splits);

/* ------------------------------- stack function family ------------------------------- */
Array* stack(Array **arrays, int num_arrays, int axis);
Array* hstack(Array **arrays, int num_arrays);
Array* dstack(Array **arrays, int num_arrays);
Array* vstack(Array **arrays, int num_arrays);
Array* column_stack(Array **arrays, int num_arrays);
// row stack, equivalent to vstack
static inline Array* row_stack(Array **arrays, int num_arrays) {
    return vstack(arrays, num_arrays);
}

// array reshaping functions
Array* reshape(Array *arr, int *new_shape, int new_ndim);
Array* resize(Array *arr, int *new_shape, int new_ndim);
Array* transpose(Array *arr);
Array* flatten(Array *arr);
Array* ravel(Array *arr, const char *order);
Array* flatnonzero(Array *arr);
Array* repeat(Array *arr, Array *repeats, int axis);
Array* roll(Array *arr, int shift, int axis);    // roll elements along an axis (axis=-1 = flattened)
Array* rot90(Array *m, int k, int axis1, int axis2);  // rotate 90 degrees counterclockwise k times
Array* take(Array *arr, Array *indices, int axis);  // take elements along an axis (numpy.take)

/* -------------------------------- flip function family -------------------------------- */
Array* flip(Array *arr, int axis);        // reverse the array along the specified axis
Array* fliplr(Array *arr);            // reverse the array horizontally (left-right flip, axis=1)
Array* flipud(Array *arr);            // reverse the array vertically (up-down flip, axis=0)


/* ------------------------------- moveaxis function family ----------------------------- */
/**
 * @brief Move an array's axis to a new position (similar to numpy.moveaxis)
 * @param a           Input array
 * @param source      Source axis index to move (supports negative indices)
 * @param destination Destination axis index (supports negative indices)
 * @return New array with the axis moved
 */
Array* moveaxis(Array *a, int source, int destination);
Array* rollaxis(Array *a, int axis, int start);    // corresponds to numpy.rollaxis
int meshgrid(Array **arrays, int n, const char *indexing, Array ***out);
Array* tile(Array *arr, int *reps, int nreps);  // construct an array by repeating (numpy.tile)
Array* squeeze(Array *arr, int axis);          // remove dimensions of length 1 (numpy.squeeze)

#ifdef __cplusplus
}
#endif