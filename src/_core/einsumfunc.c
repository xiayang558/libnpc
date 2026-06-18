#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "array.h"
#include "def.h"
#include "shape.h"
#include "einsum.h"


static const char *einsum_symbols = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
static unsigned char einsum_symbols_set[256] = {0}; // fillfunction


static void init_einsum_symbols_set(void) {
    static int initialized = 0;
    if (initialized) return;
    for (const char *p = einsum_symbols; *p; ++p) {
        einsum_symbols_set[(unsigned char)*p] = 1;
    }
    initialized = 1;
}

// helper function: checkif character is a valid symbol
static int is_valid_symbol(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

// helper function: check if string contains a character
static int contains_char(const char *s, char c) {
    for (; *s; ++s) if (*s == c) return 1;
    return 0;
}

// helper function: count occurrences of a character
static int count_char(const char *s, char c) {
    int cnt = 0;
    for (; *s; ++s) if (*s == c) cnt++;
    return cnt;
}

// helper function: replace all substrings (simplified version)
static char* replace_all(const char *src, const char *old, const char *new_str) {
    char *result = malloc(strlen(src) + 100);
    if (!result) return NULL;
    const char *p = src;
    char *q = result;
    while (*p) {
        if (strncmp(p, old, strlen(old)) == 0) {
            strcpy(q, new_str);
            q += strlen(new_str);
            p += strlen(old);
        } else {
            *q++ = *p++;
        }
    }
    *q = '\0';
    return result;
}

// internal struct definition (used for path computation)
typedef struct {
    int cost;
    int *positions;
    int num_steps;
    uint32_t *remaining;
    int remaining_len;
} ContractionState;

typedef struct {
    int sort_removed_size;
    int cost;
    int positions[2];
    uint32_t *new_input_sets;
    int new_input_sets_len;
} PossibleContraction;

typedef struct {
    int cost;
    int x, y;
    uint32_t *con_sets;
    int con_sets_len;
} ContractionResult;

// helper function: free ContractionState
static void free_contraction_state(ContractionState *state) {
    if (state->positions) free(state->positions);
    if (state->remaining) free(state->remaining);
}

// helper function: convert bitmask to string (lowercase letters)
static void mask_to_string(uint32_t mask, char *out) {
    int pos = 0;
    for (int i = 0; i < 26; i++) {
        if (mask & (1u << i)) {
            out[pos++] = 'a' + i;
        }
    }
    out[pos] = '\0';
}

// compute size product (input string)
static int _compute_size_by_dict(const char *indices, const int *idx_dict) {
    int ret = 1;
    for (int i = 0; indices[i]; ++i) {
        char c = indices[i];
        if (c >= 'a' && c <= 'z') ret *= idx_dict[c - 'a'];
    }
    return ret;
}

// find contraction info (bitmask version)
static void _find_contraction(const int *positions, int num_positions,
                              const uint32_t *input_sets, int num_inputs,
                              uint32_t output_set,
                              uint32_t *new_result,
                              uint32_t **remaining, int *remaining_len,
                              uint32_t *idx_removed,
                              uint32_t *idx_contraction) {
    uint32_t idx_contract = 0;
    uint32_t idx_remain = output_set;
    int *not_positions = malloc(num_inputs * sizeof(int));
    int not_count = 0;

    for (int i = 0; i < num_inputs; ++i) {
        int is_pos = 0;
        for (int p = 0; p < num_positions; ++p) {
            if (positions[p] == i) { is_pos = 1; break; }
        }
        if (is_pos) {
            idx_contract |= input_sets[i];
        } else {
            not_positions[not_count++] = i;
            idx_remain |= input_sets[i];
        }
    }

    *new_result = idx_remain & idx_contract;
    *idx_removed = idx_contract & ~(*new_result);
    *idx_contraction = idx_contract;

    *remaining_len = not_count + 1;
    *remaining = malloc((*remaining_len) * sizeof(uint32_t));
    for (int i = 0; i < not_count; ++i) (*remaining)[i] = input_sets[not_positions[i]];
    (*remaining)[not_count] = *new_result;

    free(not_positions);
}

// compute floating point operation count
static int64_t _flop_count(const char *idx_contraction, int inner,
                           int num_terms, const int *size_dictionary) {
    int overall_size = _compute_size_by_dict(idx_contraction, size_dictionary);
    int op_factor = (num_terms - 1);
    if (op_factor < 1) op_factor = 1;
    if (inner) op_factor += 1;
    return (int64_t)overall_size * op_factor;
}

// check whether BLAS can be used (actually unused, but keep definition)
static int _can_dot(const char **inputs, const char *result, uint32_t idx_removed) {
    // simple implementation, always return 0 (do not use BLAS)
    return 0;
}

// parse possible contractions (Return PossibleContraction*)
static PossibleContraction* _parse_possible_contraction(
    const int *positions,
    const uint32_t *input_sets, int num_inputs,
    uint32_t output_set,
    const int *idx_dict,
    int memory_limit,
    int path_cost,
    int naive_cost) {

    uint32_t new_result, idx_removed, idx_contract;
    uint32_t *new_input_sets = NULL;
    int new_len;
    _find_contraction(positions, 2, input_sets, num_inputs, output_set,
                      &new_result, &new_input_sets, &new_len,
                      &idx_removed, &idx_contract);
    if (!new_input_sets) return NULL;

    char new_res_str[27];
    mask_to_string(new_result, new_res_str);
    int new_size = _compute_size_by_dict(new_res_str, idx_dict);
    if (new_size > memory_limit) {
        free(new_input_sets);
        return NULL;
    }

    int old_sizes[2];
    for (int i = 0; i < 2; i++) {
        char set_str[27];
        mask_to_string(input_sets[positions[i]], set_str);
        old_sizes[i] = _compute_size_by_dict(set_str, idx_dict);
    }
    int removed_size = old_sizes[0] + old_sizes[1] - new_size;

    char contract_str[27];
    mask_to_string(idx_contract, contract_str);
    int inner = (idx_removed != 0) ? 1 : 0;
    int cost = _flop_count(contract_str, inner, 2, idx_dict);

    if (path_cost + cost > naive_cost) {
        free(new_input_sets);
        return NULL;
    }

    PossibleContraction *result = malloc(sizeof(PossibleContraction));
    if (!result) { free(new_input_sets); return NULL; }
    result->sort_removed_size = -removed_size;
    result->cost = cost;
    result->positions[0] = positions[0];
    result->positions[1] = positions[1];
    result->new_input_sets = new_input_sets;
    result->new_input_sets_len = new_len;
    return result;
}

// update other results (placeholder, not fully used, only keep interface)
static void _update_other_results(const ContractionResult *results, int results_len,
                                  const ContractionResult *best,
                                  ContractionResult **mod_results, int *mod_len) {
    // simplified to do nothing
    *mod_results = NULL;
    *mod_len = 0;
}

// Greedy path
static int _greedy_path(const uint32_t *input_sets, int num_inputs,
                        uint32_t output_set, const int *idx_dict,
                        int memory_limit, int **path_out, int *path_len) {
    if (num_inputs == 1) {
        *path_len = 1;
        *path_out = malloc(sizeof(int) * 2);
        (*path_out)[0] = 0; (*path_out)[1] = -1;
        return 0;
    }
    if (num_inputs == 2) {
        *path_len = 1;
        *path_out = malloc(sizeof(int) * 2);
        (*path_out)[0] = 0; (*path_out)[1] = 1;
        return 0;
    }

    // compute naive_cost
    int *all_positions = malloc(num_inputs * sizeof(int));
    for (int i = 0; i < num_inputs; i++) all_positions[i] = i;
    uint32_t new_result, idx_removed, idx_contract;
    uint32_t *new_input_sets = NULL;
    int new_len;
    _find_contraction(all_positions, num_inputs, input_sets, num_inputs,
                      output_set, &new_result, &new_input_sets, &new_len,
                      &idx_removed, &idx_contract);
    char all_idx_str[27];
    mask_to_string(idx_contract, all_idx_str);
    int inner = (idx_removed != 0) ? 1 : 0;
    int naive_cost = _flop_count(all_idx_str, inner, num_inputs, idx_dict);
    free(new_input_sets);
    free(all_positions);

    int *path = NULL;
    int path_used = 0;
    int current_cost = 0;
    uint32_t *curr_sets = malloc(num_inputs * sizeof(uint32_t));
    memcpy(curr_sets, input_sets, num_inputs * sizeof(uint32_t));
    int curr_len = num_inputs;

    for (int iter = 0; iter < num_inputs - 1; ++iter) {
        PossibleContraction *candidates = NULL;
        int cand_count = 0;

        // Inner product first
        for (int i = 0; i < curr_len; ++i) {
            for (int j = i+1; j < curr_len; ++j) {
                if ((curr_sets[i] & curr_sets[j]) == 0) continue;
                int pos[2] = {i, j};
                PossibleContraction *pc = _parse_possible_contraction(
                    pos, curr_sets, curr_len, output_set, idx_dict,
                    memory_limit, current_cost, naive_cost);
                if (pc) {
                    candidates = realloc(candidates, (cand_count+1) * sizeof(PossibleContraction));
                    candidates[cand_count++] = *pc;
                }
            }
        }

        if (cand_count == 0) {
            // Try outer product
            for (int i = 0; i < curr_len; ++i) {
                for (int j = i+1; j < curr_len; ++j) {
                    int pos[2] = {i, j};
                    PossibleContraction *pc = _parse_possible_contraction(
                        pos, curr_sets, curr_len, output_set, idx_dict,
                        memory_limit, current_cost, naive_cost);
                    if (pc) {
                        candidates = realloc(candidates, (cand_count+1) * sizeof(PossibleContraction));
                        candidates[cand_count++] = *pc;
                    }
                }
            }
            if (cand_count == 0) {
                // Fallback: directly merge all remaining
                path = realloc(path, (path_used+1) * sizeof(int)*2);
                path[path_used*2] = 0;
                path[path_used*2+1] = curr_len-1;
                path_used++;
                break;
            }
        }

        // Select best
        int best_idx = 0;
        for (int i = 1; i < cand_count; ++i) {
            if (candidates[i].sort_removed_size > candidates[best_idx].sort_removed_size ||
                (candidates[i].sort_removed_size == candidates[best_idx].sort_removed_size &&
                 candidates[i].cost < candidates[best_idx].cost)) {
                best_idx = i;
            }
        }
        PossibleContraction best = candidates[best_idx];

        path = realloc(path, (path_used+1) * sizeof(int)*2);
        path[path_used*2] = best.positions[0];
        path[path_used*2+1] = best.positions[1];
        path_used++;

        current_cost += best.cost;
        free(curr_sets);
        curr_sets = best.new_input_sets;
        curr_len = best.new_input_sets_len;

        for (int i = 0; i < cand_count; ++i) {
            if (i != best_idx) free(candidates[i].new_input_sets);
        }
        free(candidates);
    }

    *path_len = path_used;
    *path_out = path;
    free(curr_sets);
    return 0;
}

// Parse einsum input (only supports string format)
static int _parse_einsum_input(const char *subscripts, Array **operands, int num_operands,
                               char **out_input, char **out_output, Array ***out_operands) {
    init_einsum_symbols_set();
    if (num_operands == 0) {
        fprintf(stderr, "No input operands\n");
        return -1;
    }

    // Remove whitespace
    char *clean_sub = malloc(strlen(subscripts) + 1);
    if (!clean_sub) return -1;
    int idx = 0;
    for (const char *p = subscripts; *p; ++p) {
        if (!isspace(*p)) clean_sub[idx++] = *p;
    }
    clean_sub[idx] = '\0';

    // Check character validity
    for (const char *p = clean_sub; *p; ++p) {
        char c = *p;
        if (c == '.' || c == ',' || c == '-' || c == '>') continue;
        if (!einsum_symbols_set[(unsigned char)c]) {
            fprintf(stderr, "Invalid character '%c'\n", c);
            free(clean_sub);
            return -1;
        }
    }

    // handle "->"
    char *input_part, *output_part;
    const char *arrow = strstr(clean_sub, "->");
    if (arrow) {
        input_part = strndup(clean_sub, arrow - clean_sub);
        output_part = strdup(arrow + 2);
    } else {
        input_part = strdup(clean_sub);
        // Auto-generate output
        int counts[26] = {0};
        for (const char *p = input_part; *p; ++p) if (p[0] >= 'a' && p[0] <= 'z') counts[p[0]-'a']++;
        char out[27] = {0};
        int olen = 0;
        for (int i=0; i<26; ++i) if (counts[i]==1) out[olen++] = 'a'+i;
        output_part = strdup(out);
    }
    free(clean_sub);

    // Validate operand count
    int num_terms = 1;
    for (const char *p = input_part; *p; ++p) if (*p == ',') num_terms++;
    if (num_terms != num_operands) {
        fprintf(stderr, "Number of subscripts does not match number of operands\n");
        free(input_part); free(output_part);
        return -1;
    }

    // Validate output characters
    for (const char *p = output_part; *p; ++p) {
        if (!contains_char(input_part, *p)) {
            fprintf(stderr, "Output character '%c' not in input\n", *p);
            free(input_part); free(output_part);
            return -1;
        }
    }

    *out_input = input_part;
    *out_output = output_part;
    *out_operands = operands;
    return 0;
}

// Main function: einsum_path
int einsum_path(const char *subscripts, Array **operands, int num_operands,
                const char *optimize, int ***out_path, int *out_path_len,
                char **out_info) {
    char *input_sub, *output_sub;
    Array **parsed_operands;
    if (_parse_einsum_input(subscripts, operands, num_operands,
                            &input_sub, &output_sub, &parsed_operands) != 0) {
        return -1;
    }

    // Split input subscripts
    char *input_list[32];
    int num_terms = 0;
    char *token = strtok(input_sub, ",");
    while (token && num_terms < 32) {
        input_list[num_terms++] = token;
        token = strtok(NULL, ",");
    }
    if (num_terms != num_operands) {
        fprintf(stderr, "Mismatch after parsing\n");
        free(input_sub); free(output_sub);
        return -1;
    }

    // Build input sets, output set, ndim dictionary
    uint32_t *input_sets = malloc(num_operands * sizeof(uint32_t));
    uint32_t output_set = 0;
    int idx_dict[26] = {0};
    for (int i = 0; i < num_operands; ++i) {
        const char *term = input_list[i];
        Array *arr = operands[i];
        int len = strlen(term);
        if (len != arr->ndim) {
            fprintf(stderr, "Dimension mismatch for operand %d\n", i);
            free(input_sets); free(input_sub); free(output_sub);
            return -1;
        }
        uint32_t set = 0;
        for (int j = 0; j < len; ++j) {
            char c = term[j];
            if (c < 'a' || c > 'z') continue;
            set |= (1u << (c-'a'));
            int dim = arr->shape[j];
            if (idx_dict[c-'a'] == 0) idx_dict[c-'a'] = dim;
            else if (idx_dict[c-'a'] != dim && idx_dict[c-'a'] != 1 && dim != 1) {
                fprintf(stderr, "Dimension mismatch for '%c'\n", c);
                free(input_sets); free(input_sub); free(output_sub);
                return -1;
            }
            if (dim > idx_dict[c-'a']) idx_dict[c-'a'] = dim;
        }
        input_sets[i] = set;
    }
    for (const char *p = output_sub; *p; ++p) {
        if (*p >= 'a' && *p <= 'z') output_set |= (1u << (*p-'a'));
    }

    // Memory limit
    int64_t max_size = 0;
    for (int i = 0; i < num_operands; ++i) {
        int sz = _compute_size_by_dict(input_list[i], idx_dict);
        if (sz > max_size) max_size = sz;
    }
    int out_sz = _compute_size_by_dict(output_sub, idx_dict);
    if (out_sz > max_size) max_size = out_sz;
    int memory_limit = (int)max_size;

    // compute naive_cost
    uint32_t all_idx = 0;
    for (int i = 0; i < num_operands; ++i) all_idx |= input_sets[i];
    uint32_t idx_removed_all = all_idx & ~output_set;
    char all_idx_str[27], removed_str[27];
    mask_to_string(all_idx, all_idx_str);
    mask_to_string(idx_removed_all, removed_str);
    int inner = (idx_removed_all != 0) ? 1 : 0;
    int naive_cost = _flop_count(all_idx_str, inner, num_operands, idx_dict);

    int *path_flat = NULL;
    int path_len = 0;
    if (!optimize || strcmp(optimize, "false")==0 || strcmp(optimize, "False")==0) {
        path_len = 1;
        path_flat = malloc(2*sizeof(int));
        path_flat[0] = 0; path_flat[1] = num_operands-1;
    } else if (strcmp(optimize, "greedy") == 0) {
        if (_greedy_path(input_sets, num_operands, output_set, idx_dict,
                         memory_limit, &path_flat, &path_len) != 0) {
            free(input_sets); free(input_sub); free(output_sub);
            return -1;
        }
    } else {
        fprintf(stderr, "Unknown optimize '%s', only 'greedy' and 'false' supported\n", optimize);
        free(input_sets); free(input_sub); free(output_sub);
        return -1;
    }

    // Convert path format
    *out_path = malloc((path_len+1) * sizeof(int*));
    for (int i = 0; i < path_len; ++i) {
        (*out_path)[i] = malloc(2 * sizeof(int));
        (*out_path)[i][0] = path_flat[i*2];
        (*out_path)[i][1] = path_flat[i*2+1];
    }
    (*out_path)[path_len] = NULL;
    *out_path_len = path_len;

    char info[256];
    snprintf(info, sizeof(info), "einsum_path: %d steps, naive flops=%d (C implementation)", path_len, naive_cost);
    *out_info = strdup(info);

    free(path_flat);
    free(input_sets);
    free(input_sub);
    free(output_sub);
    return 0;
}


// // helper: check if string contains a character
// static int contains_char(const char *s, char c) {
//     for (; *s; ++s) if (*s == c) return 1;
//     return 0;
// }

// helper: Create scalar array (for axis subscript)
static Array* create_scalar_array_int(int val) {
    Array *arr = malloc(sizeof(Array));
    if (!arr) return NULL;
    arr->ndim = 0;
    arr->shape = NULL;
    arr->size = 1;
    arr->dtype = INT32;
    arr->data = malloc(sizeof(int));
    if (!arr->data) { free(arr); return NULL; }
    *(int*)arr->data = val;
    arr->strides = NULL;
    arr->is_view = 0;
    return arr;
}

// Corrected einsum function
Array* einsum(const char *subscripts, Array **operands, int num_operands,
              Array *out, const char *optimize) {
    // 1. Parse subscripts (Use existing _parse_einsum_input, but that function returns operand arrays; here we only need the subscript part)
    char *input_sub, *output_sub;
    Array **parsed_ops;
    if (_parse_einsum_input(subscripts, operands, num_operands,
                            &input_sub, &output_sub, &parsed_ops) != 0) {
        return NULL;
    }

    // 2. Handle single operand case (trace, diagonal, etc.)
    if (num_operands == 1) {
        Array *a = operands[0];
        // If subscript has repeated letters and output is a single letter, take the diagonal
        // instancesuch as "ii->i"
        int has_repeat = 0;
        char repeated_char = 0;
        for (int i = 0; input_sub[i]; ++i) {
            if (contains_char(input_sub + i + 1, input_sub[i])) {
                has_repeat = 1;
                repeated_char = input_sub[i];
                break;
            }
        }
        if (has_repeat && strlen(output_sub) == 1 && output_sub[0] == repeated_char) {
            // Take the diagonal
            int diag_len = a->shape[0]; // simplified: assume square matrix
            Array *result = create_array((int[]){diag_len}, 1, a->dtype);
            if (!result) return NULL;
            size_t elem_sz = dtype_size(a->dtype);
            for (int i = 0; i < diag_len; ++i) {
                int indices[2] = {i, i};
                void *src = get_item(a, indices);
                memcpy((char*)result->data + i * elem_sz, src, elem_sz);
            }
            free(input_sub); free(output_sub);
            if (out) {
                memcpy(out->data, result->data, diag_len * elem_sz);
                free_array(result);
                return out;
            }
            return result;
        } else {
            // Other cases (e.g. transpose)not yet implemented, return a copy of the original array
            Array *result = copy_array(a);
            free(input_sub); free(output_sub);
            if (out) {
                memcpy(out->data, result->data, result->size * dtype_size(result->dtype));
                free_array(result);
                return out;
            }
            return result;
        }
    }

    // 3. Two operand case (matrix multiply, dot product, outer product, etc.)
    if (num_operands == 2) {
        char *a_sub = strtok(input_sub, ",");
        char *b_sub = strtok(NULL, ",");
        if (!a_sub || !b_sub) {
            fprintf(stderr, "einsum: invalid subscripts for two operands\n");
            free(input_sub); free(output_sub);
            return NULL;
        }
        // Collect contraction axes (letters appearing in both a_sub and b_sub but not in output)
        int axes_a[32], axes_b[32];
        int n_axes = 0;
        for (int i = 0; a_sub[i]; ++i) {
            char c = a_sub[i];
            if (contains_char(b_sub, c) && !contains_char(output_sub, c)) {
                axes_a[n_axes] = i;
                for (int j = 0; b_sub[j]; ++j) {
                    if (b_sub[j] == c) {
                        axes_b[n_axes] = j;
                        break;
                    }
                }
                n_axes++;
            }
        }
        Array *result = NULL;
        if (n_axes == 0) {
            // No contraction axes → outerproduct (need to reshape both arrays to vectors then outer product, or directly call outer)
            // simplified here: call outer function
            result = outer(operands[0], operands[1]);
        } else {
            // Has contraction axes → Call tensordot
            Array *axes_a_arr = NULL, *axes_b_arr = NULL;
            if (n_axes == 1) {
                axes_a_arr = create_scalar_array_int(axes_a[0]);
                axes_b_arr = create_scalar_array_int(axes_b[0]);
            } else {
                axes_a_arr = create_array((int[]){n_axes}, 1, INT32);
                axes_b_arr = create_array((int[]){n_axes}, 1, INT32);
                memcpy(axes_a_arr->data, axes_a, n_axes * sizeof(int));
                memcpy(axes_b_arr->data, axes_b, n_axes * sizeof(int));
            }
            result = tensordot(operands[0], operands[1], axes_a_arr, axes_b_arr);
            free_array(axes_a_arr);
            free_array(axes_b_arr);
        }
        free(input_sub); free(output_sub);
        if (out) {
            memcpy(out->data, result->data, result->size * dtype_size(result->dtype));
            free_array(result);
            return out;
        }
        return result;
    }

    // More than two operands, not yet implemented
    fprintf(stderr, "einsum: more than 2 operands not fully implemented\n");
    free(input_sub); free(output_sub);
    return NULL;
}