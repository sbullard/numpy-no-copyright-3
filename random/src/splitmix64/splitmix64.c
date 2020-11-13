/* 
*/

/* This is a fixed-increment version of Java 8's SplittableRandom generator
   See http://dx.doi.org/10.1145/2714064.2660195 and
   http://docs.oracle.com/javase/8/docs/api/java/util/SplittableRandom.html

   It is a very fast generator passing BigCrush, and it can be useful if
   for some reason you absolutely want 64 bits of state; otherwise, we
   rather suggest to use a xoroshiro128+ (for moderately parallel
   computations) or xorshift1024* (for massively parallel computations)
   generator. */

#include "splitmix64.h"

extern inline uint64_t splitmix64_next(uint64_t *state);

extern inline uint64_t splitmix64_next64(splitmix64_state *state);

extern inline uint32_t splitmix64_next32(splitmix64_state *state);
