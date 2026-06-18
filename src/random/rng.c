/**
 * rng.c — xoshiro256** pseudo-random number generator
 *
 * Provides the core RNG engine for the random module.
 * Uses the xoshiro256** algorithm: fast, high-quality, period 2^256-1.
 * Seeded via splitmix64 for good state distribution from a single uint64.
 */

#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "random.h"
#include "array.h"

/* Four 64-bit words of xoshiro256** state */
static uint64_t s[4];

/** Left-rotate a 64-bit word by k bits */
static inline uint64_t rotl(uint64_t x, int k) {
    return (x << k) | (x >> (64 - k));
}

/** splitmix64 — expand a single seed into full PRNG state */
static uint64_t splitmix64(uint64_t *st) {
    uint64_t z = (*st += 0x9e3779b97f4a7c15ULL);
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
    return z ^ (z >> 31);
}

/**
 * random_seed — seed the global RNG state
 *
 * @param seed  A 64-bit seed value
 */
void random_seed(uint64_t seed) {
    uint64_t st = seed;
    s[0] = splitmix64(&st);
    s[1] = splitmix64(&st);
    s[2] = splitmix64(&st);
    s[3] = splitmix64(&st);
}

/**
 * random_uint64 — generate a uniformly distributed uint64
 *
 * Implements the xoshiro256** algorithm.
 *
 * @return A pseudo-random uint64 value
 */
uint64_t random_uint64(void) {
    const uint64_t result = rotl(s[1] * 5, 7) * 9;
    const uint64_t t = s[1] << 17;

    s[2] ^= s[0];
    s[3] ^= s[1];
    s[1] ^= s[2];
    s[0] ^= s[3];

    s[2] ^= t;
    s[3] = rotl(s[3], 45);

    return result;
}

/**
 * random_double — generate a uniformly distributed double in [0, 1)
 *
 * @return A pseudo-random double in the half-open interval [0, 1)
 */
double random_double(void) {
    return (random_uint64() >> 11) * 0x1.0p-53;
}

/* Auto-initialize state with default values if random_seed is never called */
__attribute__((constructor))
static void _auto_seed(void) {
    s[0] = 0xdeadbeefcafe0001ULL;
    s[1] = 0xdeadbeefcafe0002ULL;
    s[2] = 0xdeadbeefcafe0003ULL;
    s[3] = 0xdeadbeefcafe0004ULL;
}
