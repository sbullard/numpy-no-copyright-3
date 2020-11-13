

#ifndef PCG_VARIANTS_H_INCLUDED
#define PCG_VARIANTS_H_INCLUDED 1

#include <inttypes.h>

#if __SIZEOF_INT128__
typedef __uint128_t pcg128_t;
#define PCG_128BIT_CONSTANT(high, low) ((((pcg128_t)high) << 64) + low)
#define PCG_HAS_128BIT_OPS 1
#endif

#if __GNUC_GNU_INLINE__ && !defined(__cplusplus)
#error Nonstandard GNU inlining semantics. Compile with -std=c99 or better.
// We could instead use macros PCG_INLINE and PCG_EXTERN_INLINE
// but better to just reject ancient C code.
#endif

#if __cplusplus
extern "C" {
#endif

/*
 * Rotate helper functions.
 */

inline uint8_t pcg_rotr_8(uint8_t value, unsigned int rot) {
/* Unfortunately, clang is kinda pathetic when it comes to properly
 * recognizing idiomatic rotate code, so for clang we actually provide
 * assembler directives (enabled with PCG_USE_INLINE_ASM).  Boo, hiss.
 */
#if PCG_USE_INLINE_ASM && __clang__ && (__x86_64__ || __i386__)
  asm("rorb   %%cl, %0" : "=r"(value) : "0"(value), "c"(rot));
  return value;
#else
  return (value >> rot) | (value << ((-rot) & 7));
#endif
}

inline uint16_t pcg_rotr_16(uint16_t value, unsigned int rot) {
#if PCG_USE_INLINE_ASM && __clang__ && (__x86_64__ || __i386__)
  asm("rorw   %%cl, %0" : "=r"(value) : "0"(value), "c"(rot));
  return value;
#else
  return (value >> rot) | (value << ((-rot) & 15));
#endif
}

inline uint32_t pcg_rotr_32(uint32_t value, unsigned int rot) {
#if PCG_USE_INLINE_ASM && __clang__ && (__x86_64__ || __i386__)
  asm("rorl   %%cl, %0" : "=r"(value) : "0"(value), "c"(rot));
  return value;
#else
  return (value >> rot) | (value << ((-rot) & 31));
#endif
}

inline uint64_t pcg_rotr_64(uint64_t value, unsigned int rot) {
#if 0 && PCG_USE_INLINE_ASM && __clang__ && __x86_64__
    // For whatever reason, clang actually *does* generate rotq by
    // itself, so we don't need this code.
    asm ("rorq   %%cl, %0" : "=r" (value) : "0" (value), "c" (rot));
    return value;
#else
  return (value >> rot) | (value << ((-rot) & 63));
#endif
}

#if PCG_HAS_128BIT_OPS
inline pcg128_t pcg_rotr_128(pcg128_t value, unsigned int rot) {
  return (value >> rot) | (value << ((-rot) & 127));
}
#endif

/*
 * Output functions.  These are the core of the PCG generation scheme.
 */

// XSH RS

inline uint8_t pcg_output_xsh_rs_16_8(uint16_t state) {
  return (uint8_t)(((state >> 7u) ^ state) >> ((state >> 14u) + 3u));
}

inline uint16_t pcg_output_xsh_rs_32_16(uint32_t state) {
  return (uint16_t)(((state >> 11u) ^ state) >> ((state >> 30u) + 11u));
}

inline uint32_t pcg_output_xsh_rs_64_32(uint64_t state) {

  return (uint32_t)(((state >> 22u) ^ state) >> ((state >> 61u) + 22u));
}

#if PCG_HAS_128BIT_OPS
inline uint64_t pcg_output_xsh_rs_128_64(pcg128_t state) {
  return (uint64_t)(((state >> 43u) ^ state) >> ((state >> 124u) + 45u));
}
#endif

// XSH RR

inline uint8_t pcg_output_xsh_rr_16_8(uint16_t state) {
  return pcg_rotr_8(((state >> 5u) ^ state) >> 5u, state >> 13u);
}

inline uint16_t pcg_output_xsh_rr_32_16(uint32_t state) {
  return pcg_rotr_16(((state >> 10u) ^ state) >> 12u, state >> 28u);
}

inline uint32_t pcg_output_xsh_rr_64_32(uint64_t state) {
  return pcg_rotr_32(((state >> 18u) ^ state) >> 27u, state >> 59u);
}

#if PCG_HAS_128BIT_OPS
inline uint64_t pcg_output_xsh_rr_128_64(pcg128_t state) {
  return pcg_rotr_64(((state >> 29u) ^ state) >> 58u, state >> 122u);
}
#endif

// RXS M XS

inline uint8_t pcg_output_rxs_m_xs_8_8(uint8_t state) {
  uint8_t word = ((state >> ((state >> 6u) + 2u)) ^ state) * 217u;
  return (word >> 6u) ^ word;
}

inline uint16_t pcg_output_rxs_m_xs_16_16(uint16_t state) {
  uint16_t word = ((state >> ((state >> 13u) + 3u)) ^ state) * 62169u;
  return (word >> 11u) ^ word;
}

inline uint32_t pcg_output_rxs_m_xs_32_32(uint32_t state) {
  uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
  return (word >> 22u) ^ word;
}

inline uint64_t pcg_output_rxs_m_xs_64_64(uint64_t state) {
  uint64_t word =
      ((state >> ((state >> 59u) + 5u)) ^ state) * 12605985483714917081ull;
  return (word >> 43u) ^ word;
}

#if PCG_HAS_128BIT_OPS
inline pcg128_t pcg_output_rxs_m_xs_128_128(pcg128_t state) {
  pcg128_t word =
      ((state >> ((state >> 122u) + 6u)) ^ state) *
      (PCG_128BIT_CONSTANT(17766728186571221404ULL, 12605985483714917081ULL));
  // 327738287884841127335028083622016905945
  return (word >> 86u) ^ word;
}
#endif

// XSL RR (only defined for >= 64 bits)

inline uint32_t pcg_output_xsl_rr_64_32(uint64_t state) {
  return pcg_rotr_32(((uint32_t)(state >> 32u)) ^ (uint32_t)state,
                     state >> 59u);
}

#if PCG_HAS_128BIT_OPS
inline uint64_t pcg_output_xsl_rr_128_64(pcg128_t state) {
  return pcg_rotr_64(((uint64_t)(state >> 64u)) ^ (uint64_t)state,
                     state >> 122u);
}
#endif

// XSL RR RR (only defined for >= 64 bits)

inline uint64_t pcg_output_xsl_rr_rr_64_64(uint64_t state) {
  uint32_t rot1 = (uint32_t)(state >> 59u);
  uint32_t high = (uint32_t)(state >> 32u);
  uint32_t low = (uint32_t)state;
  uint32_t xored = high ^ low;
  uint32_t newlow = pcg_rotr_32(xored, rot1);
  uint32_t newhigh = pcg_rotr_32(high, newlow & 31u);
  return (((uint64_t)newhigh) << 32u) | newlow;
}

#if PCG_HAS_128BIT_OPS
inline pcg128_t pcg_output_xsl_rr_rr_128_128(pcg128_t state) {
  uint32_t rot1 = (uint32_t)(state >> 122u);
  uint64_t high = (uint64_t)(state >> 64u);
  uint64_t low = (uint64_t)state;
  uint64_t xored = high ^ low;
  uint64_t newlow = pcg_rotr_64(xored, rot1);
  uint64_t newhigh = pcg_rotr_64(high, newlow & 63u);
  return (((pcg128_t)newhigh) << 64u) | newlow;
}
#endif

#define PCG_DEFAULT_MULTIPLIER_8 141U
#define PCG_DEFAULT_MULTIPLIER_16 12829U
#define PCG_DEFAULT_MULTIPLIER_32 747796405U
#define PCG_DEFAULT_MULTIPLIER_64 6364136223846793005ULL

#define PCG_DEFAULT_INCREMENT_8 77U
#define PCG_DEFAULT_INCREMENT_16 47989U
#define PCG_DEFAULT_INCREMENT_32 2891336453U
#define PCG_DEFAULT_INCREMENT_64 1442695040888963407ULL

#if PCG_HAS_128BIT_OPS
#define PCG_DEFAULT_MULTIPLIER_128                                             \
  PCG_128BIT_CONSTANT(2549297995355413924ULL, 4865540595714422341ULL)
#define PCG_DEFAULT_INCREMENT_128                                              \
  PCG_128BIT_CONSTANT(6364136223846793005ULL, 1442695040888963407ULL)
#endif

  /*
   * Static initialization constants (if you can't call srandom for some
   * bizarre reason).
   */

#define PCG_STATE_ONESEQ_8_INITIALIZER                                         \
  { 0xd7U }
#define PCG_STATE_ONESEQ_16_INITIALIZER                                        \
  { 0x20dfU }
#define PCG_STATE_ONESEQ_32_INITIALIZER                                        \
  { 0x46b56677U }
#define PCG_STATE_ONESEQ_64_INITIALIZER                                        \
  { 0x4d595df4d0f33173ULL }
#if PCG_HAS_128BIT_OPS
#define PCG_STATE_ONESEQ_128_INITIALIZER                                       \
  { PCG_128BIT_CONSTANT(0xb8dc10e158a92392ULL, 0x98046df007ec0a53ULL) }
#endif

#define PCG_STATE_UNIQUE_8_INITIALIZER PCG_STATE_ONESEQ_8_INITIALIZER
#define PCG_STATE_UNIQUE_16_INITIALIZER PCG_STATE_ONESEQ_16_INITIALIZER
#define PCG_STATE_UNIQUE_32_INITIALIZER PCG_STATE_ONESEQ_32_INITIALIZER
#define PCG_STATE_UNIQUE_64_INITIALIZER PCG_STATE_ONESEQ_64_INITIALIZER
#if PCG_HAS_128BIT_OPS
#define PCG_STATE_UNIQUE_128_INITIALIZER PCG_STATE_ONESEQ_128_INITIALIZER
#endif

#define PCG_STATE_MCG_8_INITIALIZER                                            \
  { 0xe5U }
#define PCG_STATE_MCG_16_INITIALIZER                                           \
  { 0xa5e5U }
#define PCG_STATE_MCG_32_INITIALIZER                                           \
  { 0xd15ea5e5U }
#define PCG_STATE_MCG_64_INITIALIZER                                           \
  { 0xcafef00dd15ea5e5ULL }
#if PCG_HAS_128BIT_OPS
#define PCG_STATE_MCG_128_INITIALIZER                                          \
  { PCG_128BIT_CONSTANT(0x0000000000000000ULL, 0xcafef00dd15ea5e5ULL) }
#endif

#define PCG_STATE_SETSEQ_8_INITIALIZER                                         \
  { 0x9bU, 0xdbU }
#define PCG_STATE_SETSEQ_16_INITIALIZER                                        \
  { 0xe39bU, 0x5bdbU }
#define PCG_STATE_SETSEQ_32_INITIALIZER                                        \
  { 0xec02d89bU, 0x94b95bdbU }
#define PCG_STATE_SETSEQ_64_INITIALIZER                                        \
  { 0x853c49e6748fea9bULL, 0xda3e39cb94b95bdbULL }
#if PCG_HAS_128BIT_OPS
#define PCG_STATE_SETSEQ_128_INITIALIZER                                       \
  {                                                                            \
    PCG_128BIT_CONSTANT(0x979c9a98d8462005ULL, 0x7d3e9cb6cfe0549bULL)          \
    , PCG_128BIT_CONSTANT(0x0000000000000001ULL, 0xda3e39cb94b95bdbULL)        \
  }
#endif

/* Representations for the oneseq, mcg, and unique variants */

struct pcg_state_8 {
  uint8_t state;
};

struct pcg_state_16 {
  uint16_t state;
};

struct pcg_state_32 {
  uint32_t state;
};

struct pcg_state_64 {
  uint64_t state;
};

#if PCG_HAS_128BIT_OPS
struct pcg_state_128 {
  pcg128_t state;
};
#endif

/* Representations setseq variants */

struct pcg_state_setseq_8 {
  uint8_t state;
  uint8_t inc;
};

struct pcg_state_setseq_16 {
  uint16_t state;
  uint16_t inc;
};

struct pcg_state_setseq_32 {
  uint32_t state;
  uint32_t inc;
};

struct pcg_state_setseq_64 {
  uint64_t state;
  uint64_t inc;
};

#if PCG_HAS_128BIT_OPS
struct pcg_state_setseq_128 {
  pcg128_t state;
  pcg128_t inc;
};
#endif

/* Multi-step advance functions (jump-ahead, jump-back) */

extern uint8_t pcg_advance_lcg_8(uint8_t state, uint8_t delta, uint8_t cur_mult,
                                 uint8_t cur_plus);
extern uint16_t pcg_advance_lcg_16(uint16_t state, uint16_t delta,
                                   uint16_t cur_mult, uint16_t cur_plus);
extern uint32_t pcg_advance_lcg_32(uint32_t state, uint32_t delta,
                                   uint32_t cur_mult, uint32_t cur_plus);
extern uint64_t pcg_advance_lcg_64(uint64_t state, uint64_t delta,
                                   uint64_t cur_mult, uint64_t cur_plus);

#if PCG_HAS_128BIT_OPS
extern pcg128_t pcg_advance_lcg_128(pcg128_t state, pcg128_t delta,
                                    pcg128_t cur_mult, pcg128_t cur_plus);
#endif

/* Functions to advance the underlying LCG, one version for each size and
 * each style.  These functions are considered semi-private.  There is rarely
 * a good reason to call them directly.
 */

inline void pcg_oneseq_8_step_r(struct pcg_state_8 *rng) {
  rng->state = rng->state * PCG_DEFAULT_MULTIPLIER_8 + PCG_DEFAULT_INCREMENT_8;
}

inline void pcg_oneseq_8_advance_r(struct pcg_state_8 *rng, uint8_t delta) {
  rng->state = pcg_advance_lcg_8(rng->state, delta, PCG_DEFAULT_MULTIPLIER_8,
                                 PCG_DEFAULT_INCREMENT_8);
}

inline void pcg_mcg_8_step_r(struct pcg_state_8 *rng) {
  rng->state = rng->state * PCG_DEFAULT_MULTIPLIER_8;
}

inline void pcg_mcg_8_advance_r(struct pcg_state_8 *rng, uint8_t delta) {
  rng->state =
      pcg_advance_lcg_8(rng->state, delta, PCG_DEFAULT_MULTIPLIER_8, 0u);
}

inline void pcg_unique_8_step_r(struct pcg_state_8 *rng) {
  rng->state =
      rng->state * PCG_DEFAULT_MULTIPLIER_8 + (uint8_t)(((intptr_t)rng) | 1u);
}

inline void pcg_unique_8_advance_r(struct pcg_state_8 *rng, uint8_t delta) {
  rng->state = pcg_advance_lcg_8(rng->state, delta, PCG_DEFAULT_MULTIPLIER_8,
                                 (uint8_t)(((intptr_t)rng) | 1u));
}

inline void pcg_setseq_8_step_r(struct pcg_state_setseq_8 *rng) {
  rng->state = rng->state * PCG_DEFAULT_MULTIPLIER_8 + rng->inc;
}

inline void pcg_setseq_8_advance_r(struct pcg_state_setseq_8 *rng,
                                   uint8_t delta) {
  rng->state =
      pcg_advance_lcg_8(rng->state, delta, PCG_DEFAULT_MULTIPLIER_8, rng->inc);
}

inline void pcg_oneseq_16_step_r(struct pcg_state_16 *rng) {
  rng->state =
      rng->state * PCG_DEFAULT_MULTIPLIER_16 + PCG_DEFAULT_INCREMENT_16;
}

inline void pcg_oneseq_16_advance_r(struct pcg_state_16 *rng, uint16_t delta) {
  rng->state = pcg_advance_lcg_16(rng->state, delta, PCG_DEFAULT_MULTIPLIER_16,
                                  PCG_DEFAULT_INCREMENT_16);
}

inline void pcg_mcg_16_step_r(struct pcg_state_16 *rng) {
  rng->state = rng->state * PCG_DEFAULT_MULTIPLIER_16;
}

inline void pcg_mcg_16_advance_r(struct pcg_state_16 *rng, uint16_t delta) {
  rng->state =
      pcg_advance_lcg_16(rng->state, delta, PCG_DEFAULT_MULTIPLIER_16, 0u);
}

inline void pcg_unique_16_step_r(struct pcg_state_16 *rng) {
  rng->state =
      rng->state * PCG_DEFAULT_MULTIPLIER_16 + (uint16_t)(((intptr_t)rng) | 1u);
}

inline void pcg_unique_16_advance_r(struct pcg_state_16 *rng, uint16_t delta) {
  rng->state = pcg_advance_lcg_16(rng->state, delta, PCG_DEFAULT_MULTIPLIER_16,
                                  (uint16_t)(((intptr_t)rng) | 1u));
}

inline void pcg_setseq_16_step_r(struct pcg_state_setseq_16 *rng) {
  rng->state = rng->state * PCG_DEFAULT_MULTIPLIER_16 + rng->inc;
}

inline void pcg_setseq_16_advance_r(struct pcg_state_setseq_16 *rng,
                                    uint16_t delta) {
  rng->state = pcg_advance_lcg_16(rng->state, delta, PCG_DEFAULT_MULTIPLIER_16,
                                  rng->inc);
}

inline void pcg_oneseq_32_step_r(struct pcg_state_32 *rng) {
  rng->state =
      rng->state * PCG_DEFAULT_MULTIPLIER_32 + PCG_DEFAULT_INCREMENT_32;
}

inline void pcg_oneseq_32_advance_r(struct pcg_state_32 *rng, uint32_t delta) {
  rng->state = pcg_advance_lcg_32(rng->state, delta, PCG_DEFAULT_MULTIPLIER_32,
                                  PCG_DEFAULT_INCREMENT_32);
}

inline void pcg_mcg_32_step_r(struct pcg_state_32 *rng) {
  rng->state = rng->state * PCG_DEFAULT_MULTIPLIER_32;
}

inline void pcg_mcg_32_advance_r(struct pcg_state_32 *rng, uint32_t delta) {
  rng->state =
      pcg_advance_lcg_32(rng->state, delta, PCG_DEFAULT_MULTIPLIER_32, 0u);
}

inline void pcg_unique_32_step_r(struct pcg_state_32 *rng) {
  rng->state =
      rng->state * PCG_DEFAULT_MULTIPLIER_32 + (uint32_t)(((intptr_t)rng) | 1u);
}

inline void pcg_unique_32_advance_r(struct pcg_state_32 *rng, uint32_t delta) {
  rng->state = pcg_advance_lcg_32(rng->state, delta, PCG_DEFAULT_MULTIPLIER_32,
                                  (uint32_t)(((intptr_t)rng) | 1u));
}

inline void pcg_setseq_32_step_r(struct pcg_state_setseq_32 *rng) {
  rng->state = rng->state * PCG_DEFAULT_MULTIPLIER_32 + rng->inc;
}

inline void pcg_setseq_32_advance_r(struct pcg_state_setseq_32 *rng,
                                    uint32_t delta) {
  rng->state = pcg_advance_lcg_32(rng->state, delta, PCG_DEFAULT_MULTIPLIER_32,
                                  rng->inc);
}

inline void pcg_oneseq_64_step_r(struct pcg_state_64 *rng) {
  rng->state =
      rng->state * PCG_DEFAULT_MULTIPLIER_64 + PCG_DEFAULT_INCREMENT_64;
}

inline void pcg_oneseq_64_advance_r(struct pcg_state_64 *rng, uint64_t delta) {
  rng->state = pcg_advance_lcg_64(rng->state, delta, PCG_DEFAULT_MULTIPLIER_64,
                                  PCG_DEFAULT_INCREMENT_64);
}

inline void pcg_mcg_64_step_r(struct pcg_state_64 *rng) {
  rng->state = rng->state * PCG_DEFAULT_MULTIPLIER_64;
}

inline void pcg_mcg_64_advance_r(struct pcg_state_64 *rng, uint64_t delta) {
  rng->state =
      pcg_advance_lcg_64(rng->state, delta, PCG_DEFAULT_MULTIPLIER_64, 0u);
}

inline void pcg_unique_64_step_r(struct pcg_state_64 *rng) {
  rng->state =
      rng->state * PCG_DEFAULT_MULTIPLIER_64 + (uint64_t)(((intptr_t)rng) | 1u);
}

inline void pcg_unique_64_advance_r(struct pcg_state_64 *rng, uint64_t delta) {
  rng->state = pcg_advance_lcg_64(rng->state, delta, PCG_DEFAULT_MULTIPLIER_64,
                                  (uint64_t)(((intptr_t)rng) | 1u));
}

inline void pcg_setseq_64_step_r(struct pcg_state_setseq_64 *rng) {
  rng->state = rng->state * PCG_DEFAULT_MULTIPLIER_64 + rng->inc;
}

inline void pcg_setseq_64_advance_r(struct pcg_state_setseq_64 *rng,
                                    uint64_t delta) {
  rng->state = pcg_advance_lcg_64(rng->state, delta, PCG_DEFAULT_MULTIPLIER_64,
                                  rng->inc);
}

#if PCG_HAS_128BIT_OPS
inline void pcg_oneseq_128_step_r(struct pcg_state_128 *rng) {
  rng->state =
      rng->state * PCG_DEFAULT_MULTIPLIER_128 + PCG_DEFAULT_INCREMENT_128;
}
#endif

#if PCG_HAS_128BIT_OPS
inline void pcg_oneseq_128_advance_r(struct pcg_state_128 *rng,
                                     pcg128_t delta) {
  rng->state = pcg_advance_lcg_128(
      rng->state, delta, PCG_DEFAULT_MULTIPLIER_128, PCG_DEFAULT_INCREMENT_128);
}
#endif

#if PCG_HAS_128BIT_OPS
inline void pcg_mcg_128_step_r(struct pcg_state_128 *rng) {
  rng->state = rng->state * PCG_DEFAULT_MULTIPLIER_128;
}
#endif

#if PCG_HAS_128BIT_OPS
inline void pcg_mcg_128_advance_r(struct pcg_state_128 *rng, pcg128_t delta) {
  rng->state =
      pcg_advance_lcg_128(rng->state, delta, PCG_DEFAULT_MULTIPLIER_128, 0u);
}
#endif

#if PCG_HAS_128BIT_OPS
inline void pcg_unique_128_step_r(struct pcg_state_128 *rng) {
  rng->state = rng->state * PCG_DEFAULT_MULTIPLIER_128 +
               (pcg128_t)(((intptr_t)rng) | 1u);
}
#endif

#if PCG_HAS_128BIT_OPS
inline void pcg_unique_128_advance_r(struct pcg_state_128 *rng,
                                     pcg128_t delta) {
  rng->state =
      pcg_advance_lcg_128(rng->state, delta, PCG_DEFAULT_MULTIPLIER_128,
                          (pcg128_t)(((intptr_t)rng) | 1u));
}
#endif

#if PCG_HAS_128BIT_OPS
inline void pcg_setseq_128_step_r(struct pcg_state_setseq_128 *rng) {
  rng->state = rng->state * PCG_DEFAULT_MULTIPLIER_128 + rng->inc;
}
#endif

#if PCG_HAS_128BIT_OPS
inline void pcg_setseq_128_advance_r(struct pcg_state_setseq_128 *rng,
                                     pcg128_t delta) {
  rng->state = pcg_advance_lcg_128(rng->state, delta,
                                   PCG_DEFAULT_MULTIPLIER_128, rng->inc);
}
#endif

/* Functions to seed the RNG state, one version for each size and each
 * style.  Unlike the step functions, regular users can and should call
 * these functions.
 */

inline void pcg_oneseq_8_srandom_r(struct pcg_state_8 *rng, uint8_t initstate) {
  rng->state = 0U;
  pcg_oneseq_8_step_r(rng);
  rng->state += initstate;
  pcg_oneseq_8_step_r(rng);
}

inline void pcg_mcg_8_srandom_r(struct pcg_state_8 *rng, uint8_t initstate) {
  rng->state = initstate | 1u;
}

inline void pcg_unique_8_srandom_r(struct pcg_state_8 *rng, uint8_t initstate) {
  rng->state = 0U;
  pcg_unique_8_step_r(rng);
  rng->state += initstate;
  pcg_unique_8_step_r(rng);
}

inline void pcg_setseq_8_srandom_r(struct pcg_state_setseq_8 *rng,
                                   uint8_t initstate, uint8_t initseq) {
  rng->state = 0U;
  rng->inc = (initseq << 1u) | 1u;
  pcg_setseq_8_step_r(rng);
  rng->state += initstate;
  pcg_setseq_8_step_r(rng);
}

inline void pcg_oneseq_16_srandom_r(struct pcg_state_16 *rng,
                                    uint16_t initstate) {
  rng->state = 0U;
  pcg_oneseq_16_step_r(rng);
  rng->state += initstate;
  pcg_oneseq_16_step_r(rng);
}

inline void pcg_mcg_16_srandom_r(struct pcg_state_16 *rng, uint16_t initstate) {
  rng->state = initstate | 1u;
}

inline void pcg_unique_16_srandom_r(struct pcg_state_16 *rng,
                                    uint16_t initstate) {
  rng->state = 0U;
  pcg_unique_16_step_r(rng);
  rng->state += initstate;
  pcg_unique_16_step_r(rng);
}

inline void pcg_setseq_16_srandom_r(struct pcg_state_setseq_16 *rng,
                                    uint16_t initstate, uint16_t initseq) {
  rng->state = 0U;
  rng->inc = (initseq << 1u) | 1u;
  pcg_setseq_16_step_r(rng);
  rng->state += initstate;
  pcg_setseq_16_step_r(rng);
}

inline void pcg_oneseq_32_srandom_r(struct pcg_state_32 *rng,
                                    uint32_t initstate) {
  rng->state = 0U;
  pcg_oneseq_32_step_r(rng);
  rng->state += initstate;
  pcg_oneseq_32_step_r(rng);
}

inline void pcg_mcg_32_srandom_r(struct pcg_state_32 *rng, uint32_t initstate) {
  rng->state = initstate | 1u;
}

inline void pcg_unique_32_srandom_r(struct pcg_state_32 *rng,
                                    uint32_t initstate) {
  rng->state = 0U;
  pcg_unique_32_step_r(rng);
  rng->state += initstate;
  pcg_unique_32_step_r(rng);
}

inline void pcg_setseq_32_srandom_r(struct pcg_state_setseq_32 *rng,
                                    uint32_t initstate, uint32_t initseq) {
  rng->state = 0U;
  rng->inc = (initseq << 1u) | 1u;
  pcg_setseq_32_step_r(rng);
  rng->state += initstate;
  pcg_setseq_32_step_r(rng);
}

inline void pcg_oneseq_64_srandom_r(struct pcg_state_64 *rng,
                                    uint64_t initstate) {
  rng->state = 0U;
  pcg_oneseq_64_step_r(rng);
  rng->state += initstate;
  pcg_oneseq_64_step_r(rng);
}

inline void pcg_mcg_64_srandom_r(struct pcg_state_64 *rng, uint64_t initstate) {
  rng->state = initstate | 1u;
}

inline void pcg_unique_64_srandom_r(struct pcg_state_64 *rng,
                                    uint64_t initstate) {
  rng->state = 0U;
  pcg_unique_64_step_r(rng);
  rng->state += initstate;
  pcg_unique_64_step_r(rng);
}

inline void pcg_setseq_64_srandom_r(struct pcg_state_setseq_64 *rng,
                                    uint64_t initstate, uint64_t initseq) {
  rng->state = 0U;
  rng->inc = (initseq << 1u) | 1u;
  pcg_setseq_64_step_r(rng);
  rng->state += initstate;
  pcg_setseq_64_step_r(rng);
}

#if PCG_HAS_128BIT_OPS
inline void pcg_oneseq_128_srandom_r(struct pcg_state_128 *rng,
                                     pcg128_t initstate) {
  rng->state = 0U;
  pcg_oneseq_128_step_r(rng);
  rng->state += initstate;
  pcg_oneseq_128_step_r(rng);
}
#endif

#if PCG_HAS_128BIT_OPS
inline void pcg_mcg_128_srandom_r(struct pcg_state_128 *rng,
                                  pcg128_t initstate) {
  rng->state = initstate | 1u;
}
#endif

#if PCG_HAS_128BIT_OPS
inline void pcg_unique_128_srandom_r(struct pcg_state_128 *rng,
                                     pcg128_t initstate) {
  rng->state = 0U;
  pcg_unique_128_step_r(rng);
  rng->state += initstate;
  pcg_unique_128_step_r(rng);
}
#endif

#if PCG_HAS_128BIT_OPS
inline void pcg_setseq_128_srandom_r(struct pcg_state_setseq_128 *rng,
                                     pcg128_t initstate, pcg128_t initseq) {
  rng->state = 0U;
  rng->inc = (initseq << 1u) | 1u;
  pcg_setseq_128_step_r(rng);
  rng->state += initstate;
  pcg_setseq_128_step_r(rng);
}
#endif

/* Now, finally we create each of the individual generators. We provide
 * a random_r function that provides a random number of the appropriate
 * type (using the full range of the type) and a boundedrand_r version
 * that provides
 *
 * Implementation notes for boundedrand_r:
 *
 *     To avoid bias, we need to make the range of the RNG a multiple of
 *     bound, which we do by dropping output less than a threshold.
 *     Let's consider a 32-bit case...  A naive scheme to calculate the
 *     threshold would be to do
 *
 *         uint32_t threshold = 0x100000000ull % bound;
 *
 *     but 64-bit div/mod is slower than 32-bit div/mod (especially on
 *     32-bit platforms).  In essence, we do
 *
 *         uint32_t threshold = (0x100000000ull-bound) % bound;
 *
 *     because this version will calculate the same modulus, but the LHS
 *     value is less than 2^32.
 *
 *     (Note that using modulo is only wise for good RNGs, poorer RNGs
 *     such as raw LCGs do better using a technique based on division.)
 *     Empricical tests show that division is preferable to modulus for
 *     reducting the range of an RNG.  It's faster, and sometimes it can
 *     even be statistically prefereable.
 */

/* Generation functions for XSH RS */

inline uint8_t pcg_oneseq_16_xsh_rs_8_random_r(struct pcg_state_16 *rng) {
  uint16_t oldstate = rng->state;
  pcg_oneseq_16_step_r(rng);
  return pcg_output_xsh_rs_16_8(oldstate);
}

inline uint8_t pcg_oneseq_16_xsh_rs_8_boundedrand_r(struct pcg_state_16 *rng,
                                                    uint8_t bound) {
  uint8_t threshold = ((uint8_t)(-bound)) % bound;
  for (;;) {
    uint8_t r = pcg_oneseq_16_xsh_rs_8_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}

inline uint16_t pcg_oneseq_32_xsh_rs_16_random_r(struct pcg_state_32 *rng) {
  uint32_t oldstate = rng->state;
  pcg_oneseq_32_step_r(rng);
  return pcg_output_xsh_rs_32_16(oldstate);
}

inline uint16_t pcg_oneseq_32_xsh_rs_16_boundedrand_r(struct pcg_state_32 *rng,
                                                      uint16_t bound) {
  uint16_t threshold = ((uint16_t)(-bound)) % bound;
  for (;;) {
    uint16_t r = pcg_oneseq_32_xsh_rs_16_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}

inline uint32_t pcg_oneseq_64_xsh_rs_32_random_r(struct pcg_state_64 *rng) {
  uint64_t oldstate = rng->state;
  pcg_oneseq_64_step_r(rng);
  return pcg_output_xsh_rs_64_32(oldstate);
}

inline uint32_t pcg_oneseq_64_xsh_rs_32_boundedrand_r(struct pcg_state_64 *rng,
                                                      uint32_t bound) {
  uint32_t threshold = -bound % bound;
  for (;;) {
    uint32_t r = pcg_oneseq_64_xsh_rs_32_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}

#if PCG_HAS_128BIT_OPS
inline uint64_t pcg_oneseq_128_xsh_rs_64_random_r(struct pcg_state_128 *rng) {
  pcg_oneseq_128_step_r(rng);
  return pcg_output_xsh_rs_128_64(rng->state);
}
#endif

#if PCG_HAS_128BIT_OPS
inline uint64_t
pcg_oneseq_128_xsh_rs_64_boundedrand_r(struct pcg_state_128 *rng,
                                       uint64_t bound) {
  uint64_t threshold = -bound % bound;
  for (;;) {
    uint64_t r = pcg_oneseq_128_xsh_rs_64_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}
#endif

inline uint8_t pcg_unique_16_xsh_rs_8_random_r(struct pcg_state_16 *rng) {
  uint16_t oldstate = rng->state;
  pcg_unique_16_step_r(rng);
  return pcg_output_xsh_rs_16_8(oldstate);
}

inline uint8_t pcg_unique_16_xsh_rs_8_boundedrand_r(struct pcg_state_16 *rng,
                                                    uint8_t bound) {
  uint8_t threshold = ((uint8_t)(-bound)) % bound;
  for (;;) {
    uint8_t r = pcg_unique_16_xsh_rs_8_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}

inline uint16_t pcg_unique_32_xsh_rs_16_random_r(struct pcg_state_32 *rng) {
  uint32_t oldstate = rng->state;
  pcg_unique_32_step_r(rng);
  return pcg_output_xsh_rs_32_16(oldstate);
}

inline uint16_t pcg_unique_32_xsh_rs_16_boundedrand_r(struct pcg_state_32 *rng,
                                                      uint16_t bound) {
  uint16_t threshold = ((uint16_t)(-bound)) % bound;
  for (;;) {
    uint16_t r = pcg_unique_32_xsh_rs_16_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}

inline uint32_t pcg_unique_64_xsh_rs_32_random_r(struct pcg_state_64 *rng) {
  uint64_t oldstate = rng->state;
  pcg_unique_64_step_r(rng);
  return pcg_output_xsh_rs_64_32(oldstate);
}

inline uint32_t pcg_unique_64_xsh_rs_32_boundedrand_r(struct pcg_state_64 *rng,
                                                      uint32_t bound) {
  uint32_t threshold = -bound % bound;
  for (;;) {
    uint32_t r = pcg_unique_64_xsh_rs_32_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}

#if PCG_HAS_128BIT_OPS
inline uint64_t pcg_unique_128_xsh_rs_64_random_r(struct pcg_state_128 *rng) {
  pcg_unique_128_step_r(rng);
  return pcg_output_xsh_rs_128_64(rng->state);
}
#endif

#if PCG_HAS_128BIT_OPS
inline uint64_t
pcg_unique_128_xsh_rs_64_boundedrand_r(struct pcg_state_128 *rng,
                                       uint64_t bound) {
  uint64_t threshold = -bound % bound;
  for (;;) {
    uint64_t r = pcg_unique_128_xsh_rs_64_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}
#endif

inline uint8_t
pcg_setseq_16_xsh_rs_8_random_r(struct pcg_state_setseq_16 *rng) {
  uint16_t oldstate = rng->state;
  pcg_setseq_16_step_r(rng);
  return pcg_output_xsh_rs_16_8(oldstate);
}

inline uint8_t
pcg_setseq_16_xsh_rs_8_boundedrand_r(struct pcg_state_setseq_16 *rng,
                                     uint8_t bound) {
  uint8_t threshold = ((uint8_t)(-bound)) % bound;
  for (;;) {
    uint8_t r = pcg_setseq_16_xsh_rs_8_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}

inline uint16_t
pcg_setseq_32_xsh_rs_16_random_r(struct pcg_state_setseq_32 *rng) {
  uint32_t oldstate = rng->state;
  pcg_setseq_32_step_r(rng);
  return pcg_output_xsh_rs_32_16(oldstate);
}

inline uint16_t
pcg_setseq_32_xsh_rs_16_boundedrand_r(struct pcg_state_setseq_32 *rng,
                                      uint16_t bound) {
  uint16_t threshold = ((uint16_t)(-bound)) % bound;
  for (;;) {
    uint16_t r = pcg_setseq_32_xsh_rs_16_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}

inline uint32_t
pcg_setseq_64_xsh_rs_32_random_r(struct pcg_state_setseq_64 *rng) {
  uint64_t oldstate = rng->state;
  pcg_setseq_64_step_r(rng);
  return pcg_output_xsh_rs_64_32(oldstate);
}

inline uint32_t
pcg_setseq_64_xsh_rs_32_boundedrand_r(struct pcg_state_setseq_64 *rng,
                                      uint32_t bound) {
  uint32_t threshold = -bound % bound;
  for (;;) {
    uint32_t r = pcg_setseq_64_xsh_rs_32_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}

#if PCG_HAS_128BIT_OPS
inline uint64_t
pcg_setseq_128_xsh_rs_64_random_r(struct pcg_state_setseq_128 *rng) {
  pcg_setseq_128_step_r(rng);
  return pcg_output_xsh_rs_128_64(rng->state);
}
#endif

#if PCG_HAS_128BIT_OPS
inline uint64_t
pcg_setseq_128_xsh_rs_64_boundedrand_r(struct pcg_state_setseq_128 *rng,
                                       uint64_t bound) {
  uint64_t threshold = -bound % bound;
  for (;;) {
    uint64_t r = pcg_setseq_128_xsh_rs_64_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}
#endif

inline uint8_t pcg_mcg_16_xsh_rs_8_random_r(struct pcg_state_16 *rng) {
  uint16_t oldstate = rng->state;
  pcg_mcg_16_step_r(rng);
  return pcg_output_xsh_rs_16_8(oldstate);
}

inline uint8_t pcg_mcg_16_xsh_rs_8_boundedrand_r(struct pcg_state_16 *rng,
                                                 uint8_t bound) {
  uint8_t threshold = ((uint8_t)(-bound)) % bound;
  for (;;) {
    uint8_t r = pcg_mcg_16_xsh_rs_8_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}

inline uint16_t pcg_mcg_32_xsh_rs_16_random_r(struct pcg_state_32 *rng) {
  uint32_t oldstate = rng->state;
  pcg_mcg_32_step_r(rng);
  return pcg_output_xsh_rs_32_16(oldstate);
}

inline uint16_t pcg_mcg_32_xsh_rs_16_boundedrand_r(struct pcg_state_32 *rng,
                                                   uint16_t bound) {
  uint16_t threshold = ((uint16_t)(-bound)) % bound;
  for (;;) {
    uint16_t r = pcg_mcg_32_xsh_rs_16_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}

inline uint32_t pcg_mcg_64_xsh_rs_32_random_r(struct pcg_state_64 *rng) {
  uint64_t oldstate = rng->state;
  pcg_mcg_64_step_r(rng);
  return pcg_output_xsh_rs_64_32(oldstate);
}

inline uint32_t pcg_mcg_64_xsh_rs_32_boundedrand_r(struct pcg_state_64 *rng,
                                                   uint32_t bound) {
  uint32_t threshold = -bound % bound;
  for (;;) {
    uint32_t r = pcg_mcg_64_xsh_rs_32_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}

#if PCG_HAS_128BIT_OPS
inline uint64_t pcg_mcg_128_xsh_rs_64_random_r(struct pcg_state_128 *rng) {
  pcg_mcg_128_step_r(rng);
  return pcg_output_xsh_rs_128_64(rng->state);
}
#endif

#if PCG_HAS_128BIT_OPS
inline uint64_t pcg_mcg_128_xsh_rs_64_boundedrand_r(struct pcg_state_128 *rng,
                                                    uint64_t bound) {
  uint64_t threshold = -bound % bound;
  for (;;) {
    uint64_t r = pcg_mcg_128_xsh_rs_64_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}
#endif

/* Generation functions for XSH RR */

inline uint8_t pcg_oneseq_16_xsh_rr_8_random_r(struct pcg_state_16 *rng) {
  uint16_t oldstate = rng->state;
  pcg_oneseq_16_step_r(rng);
  return pcg_output_xsh_rr_16_8(oldstate);
}

inline uint8_t pcg_oneseq_16_xsh_rr_8_boundedrand_r(struct pcg_state_16 *rng,
                                                    uint8_t bound) {
  uint8_t threshold = ((uint8_t)(-bound)) % bound;
  for (;;) {
    uint8_t r = pcg_oneseq_16_xsh_rr_8_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}

inline uint16_t pcg_oneseq_32_xsh_rr_16_random_r(struct pcg_state_32 *rng) {
  uint32_t oldstate = rng->state;
  pcg_oneseq_32_step_r(rng);
  return pcg_output_xsh_rr_32_16(oldstate);
}

inline uint16_t pcg_oneseq_32_xsh_rr_16_boundedrand_r(struct pcg_state_32 *rng,
                                                      uint16_t bound) {
  uint16_t threshold = ((uint16_t)(-bound)) % bound;
  for (;;) {
    uint16_t r = pcg_oneseq_32_xsh_rr_16_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}

inline uint32_t pcg_oneseq_64_xsh_rr_32_random_r(struct pcg_state_64 *rng) {
  uint64_t oldstate = rng->state;
  pcg_oneseq_64_step_r(rng);
  return pcg_output_xsh_rr_64_32(oldstate);
}

inline uint32_t pcg_oneseq_64_xsh_rr_32_boundedrand_r(struct pcg_state_64 *rng,
                                                      uint32_t bound) {
  uint32_t threshold = -bound % bound;
  for (;;) {
    uint32_t r = pcg_oneseq_64_xsh_rr_32_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}

#if PCG_HAS_128BIT_OPS
inline uint64_t pcg_oneseq_128_xsh_rr_64_random_r(struct pcg_state_128 *rng) {
  pcg_oneseq_128_step_r(rng);
  return pcg_output_xsh_rr_128_64(rng->state);
}
#endif

#if PCG_HAS_128BIT_OPS
inline uint64_t
pcg_oneseq_128_xsh_rr_64_boundedrand_r(struct pcg_state_128 *rng,
                                       uint64_t bound) {
  uint64_t threshold = -bound % bound;
  for (;;) {
    uint64_t r = pcg_oneseq_128_xsh_rr_64_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}
#endif

inline uint8_t pcg_unique_16_xsh_rr_8_random_r(struct pcg_state_16 *rng) {
  uint16_t oldstate = rng->state;
  pcg_unique_16_step_r(rng);
  return pcg_output_xsh_rr_16_8(oldstate);
}

inline uint8_t pcg_unique_16_xsh_rr_8_boundedrand_r(struct pcg_state_16 *rng,
                                                    uint8_t bound) {
  uint8_t threshold = ((uint8_t)(-bound)) % bound;
  for (;;) {
    uint8_t r = pcg_unique_16_xsh_rr_8_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}

inline uint16_t pcg_unique_32_xsh_rr_16_random_r(struct pcg_state_32 *rng) {
  uint32_t oldstate = rng->state;
  pcg_unique_32_step_r(rng);
  return pcg_output_xsh_rr_32_16(oldstate);
}

inline uint16_t pcg_unique_32_xsh_rr_16_boundedrand_r(struct pcg_state_32 *rng,
                                                      uint16_t bound) {
  uint16_t threshold = ((uint16_t)(-bound)) % bound;
  for (;;) {
    uint16_t r = pcg_unique_32_xsh_rr_16_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}

inline uint32_t pcg_unique_64_xsh_rr_32_random_r(struct pcg_state_64 *rng) {
  uint64_t oldstate = rng->state;
  pcg_unique_64_step_r(rng);
  return pcg_output_xsh_rr_64_32(oldstate);
}

inline uint32_t pcg_unique_64_xsh_rr_32_boundedrand_r(struct pcg_state_64 *rng,
                                                      uint32_t bound) {
  uint32_t threshold = -bound % bound;
  for (;;) {
    uint32_t r = pcg_unique_64_xsh_rr_32_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}

#if PCG_HAS_128BIT_OPS
inline uint64_t pcg_unique_128_xsh_rr_64_random_r(struct pcg_state_128 *rng) {
  pcg_unique_128_step_r(rng);
  return pcg_output_xsh_rr_128_64(rng->state);
}
#endif

#if PCG_HAS_128BIT_OPS
inline uint64_t
pcg_unique_128_xsh_rr_64_boundedrand_r(struct pcg_state_128 *rng,
                                       uint64_t bound) {
  uint64_t threshold = -bound % bound;
  for (;;) {
    uint64_t r = pcg_unique_128_xsh_rr_64_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}
#endif

inline uint8_t
pcg_setseq_16_xsh_rr_8_random_r(struct pcg_state_setseq_16 *rng) {
  uint16_t oldstate = rng->state;
  pcg_setseq_16_step_r(rng);
  return pcg_output_xsh_rr_16_8(oldstate);
}

inline uint8_t
pcg_setseq_16_xsh_rr_8_boundedrand_r(struct pcg_state_setseq_16 *rng,
                                     uint8_t bound) {
  uint8_t threshold = ((uint8_t)(-bound)) % bound;
  for (;;) {
    uint8_t r = pcg_setseq_16_xsh_rr_8_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}

inline uint16_t
pcg_setseq_32_xsh_rr_16_random_r(struct pcg_state_setseq_32 *rng) {
  uint32_t oldstate = rng->state;
  pcg_setseq_32_step_r(rng);
  return pcg_output_xsh_rr_32_16(oldstate);
}

inline uint16_t
pcg_setseq_32_xsh_rr_16_boundedrand_r(struct pcg_state_setseq_32 *rng,
                                      uint16_t bound) {
  uint16_t threshold = ((uint16_t)(-bound)) % bound;
  for (;;) {
    uint16_t r = pcg_setseq_32_xsh_rr_16_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}

inline uint32_t
pcg_setseq_64_xsh_rr_32_random_r(struct pcg_state_setseq_64 *rng) {
  uint64_t oldstate = rng->state;
  pcg_setseq_64_step_r(rng);
  return pcg_output_xsh_rr_64_32(oldstate);
}

inline uint32_t
pcg_setseq_64_xsh_rr_32_boundedrand_r(struct pcg_state_setseq_64 *rng,
                                      uint32_t bound) {
  uint32_t threshold = -bound % bound;
  for (;;) {
    uint32_t r = pcg_setseq_64_xsh_rr_32_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}

#if PCG_HAS_128BIT_OPS
inline uint64_t
pcg_setseq_128_xsh_rr_64_random_r(struct pcg_state_setseq_128 *rng) {
  pcg_setseq_128_step_r(rng);
  return pcg_output_xsh_rr_128_64(rng->state);
}
#endif

#if PCG_HAS_128BIT_OPS
inline uint64_t
pcg_setseq_128_xsh_rr_64_boundedrand_r(struct pcg_state_setseq_128 *rng,
                                       uint64_t bound) {
  uint64_t threshold = -bound % bound;
  for (;;) {
    uint64_t r = pcg_setseq_128_xsh_rr_64_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}
#endif

inline uint8_t pcg_mcg_16_xsh_rr_8_random_r(struct pcg_state_16 *rng) {
  uint16_t oldstate = rng->state;
  pcg_mcg_16_step_r(rng);
  return pcg_output_xsh_rr_16_8(oldstate);
}

inline uint8_t pcg_mcg_16_xsh_rr_8_boundedrand_r(struct pcg_state_16 *rng,
                                                 uint8_t bound) {
  uint8_t threshold = ((uint8_t)(-bound)) % bound;
  for (;;) {
    uint8_t r = pcg_mcg_16_xsh_rr_8_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}

inline uint16_t pcg_mcg_32_xsh_rr_16_random_r(struct pcg_state_32 *rng) {
  uint32_t oldstate = rng->state;
  pcg_mcg_32_step_r(rng);
  return pcg_output_xsh_rr_32_16(oldstate);
}

inline uint16_t pcg_mcg_32_xsh_rr_16_boundedrand_r(struct pcg_state_32 *rng,
                                                   uint16_t bound) {
  uint16_t threshold = ((uint16_t)(-bound)) % bound;
  for (;;) {
    uint16_t r = pcg_mcg_32_xsh_rr_16_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}

inline uint32_t pcg_mcg_64_xsh_rr_32_random_r(struct pcg_state_64 *rng) {
  uint64_t oldstate = rng->state;
  pcg_mcg_64_step_r(rng);
  return pcg_output_xsh_rr_64_32(oldstate);
}

inline uint32_t pcg_mcg_64_xsh_rr_32_boundedrand_r(struct pcg_state_64 *rng,
                                                   uint32_t bound) {
  uint32_t threshold = -bound % bound;
  for (;;) {
    uint32_t r = pcg_mcg_64_xsh_rr_32_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}

#if PCG_HAS_128BIT_OPS
inline uint64_t pcg_mcg_128_xsh_rr_64_random_r(struct pcg_state_128 *rng) {
  pcg_mcg_128_step_r(rng);
  return pcg_output_xsh_rr_128_64(rng->state);
}
#endif

#if PCG_HAS_128BIT_OPS
inline uint64_t pcg_mcg_128_xsh_rr_64_boundedrand_r(struct pcg_state_128 *rng,
                                                    uint64_t bound) {
  uint64_t threshold = -bound % bound;
  for (;;) {
    uint64_t r = pcg_mcg_128_xsh_rr_64_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}
#endif

/* Generation functions for RXS M XS (no MCG versions because they
 * don't make sense when you want to use the entire state)
 */

inline uint8_t pcg_oneseq_8_rxs_m_xs_8_random_r(struct pcg_state_8 *rng) {
  uint8_t oldstate = rng->state;
  pcg_oneseq_8_step_r(rng);
  return pcg_output_rxs_m_xs_8_8(oldstate);
}

inline uint8_t pcg_oneseq_8_rxs_m_xs_8_boundedrand_r(struct pcg_state_8 *rng,
                                                     uint8_t bound) {
  uint8_t threshold = ((uint8_t)(-bound)) % bound;
  for (;;) {
    uint8_t r = pcg_oneseq_8_rxs_m_xs_8_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}

inline uint16_t pcg_oneseq_16_rxs_m_xs_16_random_r(struct pcg_state_16 *rng) {
  uint16_t oldstate = rng->state;
  pcg_oneseq_16_step_r(rng);
  return pcg_output_rxs_m_xs_16_16(oldstate);
}

inline uint16_t
pcg_oneseq_16_rxs_m_xs_16_boundedrand_r(struct pcg_state_16 *rng,
                                        uint16_t bound) {
  uint16_t threshold = ((uint16_t)(-bound)) % bound;
  for (;;) {
    uint16_t r = pcg_oneseq_16_rxs_m_xs_16_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}

inline uint32_t pcg_oneseq_32_rxs_m_xs_32_random_r(struct pcg_state_32 *rng) {
  uint32_t oldstate = rng->state;
  pcg_oneseq_32_step_r(rng);
  return pcg_output_rxs_m_xs_32_32(oldstate);
}

inline uint32_t
pcg_oneseq_32_rxs_m_xs_32_boundedrand_r(struct pcg_state_32 *rng,
                                        uint32_t bound) {
  uint32_t threshold = -bound % bound;
  for (;;) {
    uint32_t r = pcg_oneseq_32_rxs_m_xs_32_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}

inline uint64_t pcg_oneseq_64_rxs_m_xs_64_random_r(struct pcg_state_64 *rng) {
  uint64_t oldstate = rng->state;
  pcg_oneseq_64_step_r(rng);
  return pcg_output_rxs_m_xs_64_64(oldstate);
}

inline uint64_t
pcg_oneseq_64_rxs_m_xs_64_boundedrand_r(struct pcg_state_64 *rng,
                                        uint64_t bound) {
  uint64_t threshold = -bound % bound;
  for (;;) {
    uint64_t r = pcg_oneseq_64_rxs_m_xs_64_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}

#if PCG_HAS_128BIT_OPS
inline pcg128_t
pcg_oneseq_128_rxs_m_xs_128_random_r(struct pcg_state_128 *rng) {
  pcg_oneseq_128_step_r(rng);
  return pcg_output_rxs_m_xs_128_128(rng->state);
}
#endif

#if PCG_HAS_128BIT_OPS
inline pcg128_t
pcg_oneseq_128_rxs_m_xs_128_boundedrand_r(struct pcg_state_128 *rng,
                                          pcg128_t bound) {
  pcg128_t threshold = -bound % bound;
  for (;;) {
    pcg128_t r = pcg_oneseq_128_rxs_m_xs_128_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}
#endif

inline uint16_t pcg_unique_16_rxs_m_xs_16_random_r(struct pcg_state_16 *rng) {
  uint16_t oldstate = rng->state;
  pcg_unique_16_step_r(rng);
  return pcg_output_rxs_m_xs_16_16(oldstate);
}

inline uint16_t
pcg_unique_16_rxs_m_xs_16_boundedrand_r(struct pcg_state_16 *rng,
                                        uint16_t bound) {
  uint16_t threshold = ((uint16_t)(-bound)) % bound;
  for (;;) {
    uint16_t r = pcg_unique_16_rxs_m_xs_16_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}

inline uint32_t pcg_unique_32_rxs_m_xs_32_random_r(struct pcg_state_32 *rng) {
  uint32_t oldstate = rng->state;
  pcg_unique_32_step_r(rng);
  return pcg_output_rxs_m_xs_32_32(oldstate);
}

inline uint32_t
pcg_unique_32_rxs_m_xs_32_boundedrand_r(struct pcg_state_32 *rng,
                                        uint32_t bound) {
  uint32_t threshold = -bound % bound;
  for (;;) {
    uint32_t r = pcg_unique_32_rxs_m_xs_32_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}

inline uint64_t pcg_unique_64_rxs_m_xs_64_random_r(struct pcg_state_64 *rng) {
  uint64_t oldstate = rng->state;
  pcg_unique_64_step_r(rng);
  return pcg_output_rxs_m_xs_64_64(oldstate);
}

inline uint64_t
pcg_unique_64_rxs_m_xs_64_boundedrand_r(struct pcg_state_64 *rng,
                                        uint64_t bound) {
  uint64_t threshold = -bound % bound;
  for (;;) {
    uint64_t r = pcg_unique_64_rxs_m_xs_64_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}

#if PCG_HAS_128BIT_OPS
inline pcg128_t
pcg_unique_128_rxs_m_xs_128_random_r(struct pcg_state_128 *rng) {
  pcg_unique_128_step_r(rng);
  return pcg_output_rxs_m_xs_128_128(rng->state);
}
#endif

#if PCG_HAS_128BIT_OPS
inline pcg128_t
pcg_unique_128_rxs_m_xs_128_boundedrand_r(struct pcg_state_128 *rng,
                                          pcg128_t bound) {
  pcg128_t threshold = -bound % bound;
  for (;;) {
    pcg128_t r = pcg_unique_128_rxs_m_xs_128_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}
#endif

inline uint8_t
pcg_setseq_8_rxs_m_xs_8_random_r(struct pcg_state_setseq_8 *rng) {
  uint8_t oldstate = rng->state;
  pcg_setseq_8_step_r(rng);
  return pcg_output_rxs_m_xs_8_8(oldstate);
}

inline uint8_t
pcg_setseq_8_rxs_m_xs_8_boundedrand_r(struct pcg_state_setseq_8 *rng,
                                      uint8_t bound) {
  uint8_t threshold = ((uint8_t)(-bound)) % bound;
  for (;;) {
    uint8_t r = pcg_setseq_8_rxs_m_xs_8_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}

inline uint16_t
pcg_setseq_16_rxs_m_xs_16_random_r(struct pcg_state_setseq_16 *rng) {
  uint16_t oldstate = rng->state;
  pcg_setseq_16_step_r(rng);
  return pcg_output_rxs_m_xs_16_16(oldstate);
}

inline uint16_t
pcg_setseq_16_rxs_m_xs_16_boundedrand_r(struct pcg_state_setseq_16 *rng,
                                        uint16_t bound) {
  uint16_t threshold = ((uint16_t)(-bound)) % bound;
  for (;;) {
    uint16_t r = pcg_setseq_16_rxs_m_xs_16_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}

inline uint32_t
pcg_setseq_32_rxs_m_xs_32_random_r(struct pcg_state_setseq_32 *rng) {
  uint32_t oldstate = rng->state;
  pcg_setseq_32_step_r(rng);
  return pcg_output_rxs_m_xs_32_32(oldstate);
}

inline uint32_t
pcg_setseq_32_rxs_m_xs_32_boundedrand_r(struct pcg_state_setseq_32 *rng,
                                        uint32_t bound) {
  uint32_t threshold = -bound % bound;
  for (;;) {
    uint32_t r = pcg_setseq_32_rxs_m_xs_32_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}

inline uint64_t
pcg_setseq_64_rxs_m_xs_64_random_r(struct pcg_state_setseq_64 *rng) {
  uint64_t oldstate = rng->state;
  pcg_setseq_64_step_r(rng);
  return pcg_output_rxs_m_xs_64_64(oldstate);
}

inline uint64_t
pcg_setseq_64_rxs_m_xs_64_boundedrand_r(struct pcg_state_setseq_64 *rng,
                                        uint64_t bound) {
  uint64_t threshold = -bound % bound;
  for (;;) {
    uint64_t r = pcg_setseq_64_rxs_m_xs_64_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}

#if PCG_HAS_128BIT_OPS
inline pcg128_t
pcg_setseq_128_rxs_m_xs_128_random_r(struct pcg_state_setseq_128 *rng) {
  pcg_setseq_128_step_r(rng);
  return pcg_output_rxs_m_xs_128_128(rng->state);
}
#endif

#if PCG_HAS_128BIT_OPS
inline pcg128_t
pcg_setseq_128_rxs_m_xs_128_boundedrand_r(struct pcg_state_setseq_128 *rng,
                                          pcg128_t bound) {
  pcg128_t threshold = -bound % bound;
  for (;;) {
    pcg128_t r = pcg_setseq_128_rxs_m_xs_128_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}
#endif

/* Generation functions for XSL RR (only defined for "large" types) */

inline uint32_t pcg_oneseq_64_xsl_rr_32_random_r(struct pcg_state_64 *rng) {
  uint64_t oldstate = rng->state;
  pcg_oneseq_64_step_r(rng);
  return pcg_output_xsl_rr_64_32(oldstate);
}

inline uint32_t pcg_oneseq_64_xsl_rr_32_boundedrand_r(struct pcg_state_64 *rng,
                                                      uint32_t bound) {
  uint32_t threshold = -bound % bound;
  for (;;) {
    uint32_t r = pcg_oneseq_64_xsl_rr_32_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}

#if PCG_HAS_128BIT_OPS
inline uint64_t pcg_oneseq_128_xsl_rr_64_random_r(struct pcg_state_128 *rng) {
  pcg_oneseq_128_step_r(rng);
  return pcg_output_xsl_rr_128_64(rng->state);
}
#endif

#if PCG_HAS_128BIT_OPS
inline uint64_t
pcg_oneseq_128_xsl_rr_64_boundedrand_r(struct pcg_state_128 *rng,
                                       uint64_t bound) {
  uint64_t threshold = -bound % bound;
  for (;;) {
    uint64_t r = pcg_oneseq_128_xsl_rr_64_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}
#endif

inline uint32_t pcg_unique_64_xsl_rr_32_random_r(struct pcg_state_64 *rng) {
  uint64_t oldstate = rng->state;
  pcg_unique_64_step_r(rng);
  return pcg_output_xsl_rr_64_32(oldstate);
}

inline uint32_t pcg_unique_64_xsl_rr_32_boundedrand_r(struct pcg_state_64 *rng,
                                                      uint32_t bound) {
  uint32_t threshold = -bound % bound;
  for (;;) {
    uint32_t r = pcg_unique_64_xsl_rr_32_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}

#if PCG_HAS_128BIT_OPS
inline uint64_t pcg_unique_128_xsl_rr_64_random_r(struct pcg_state_128 *rng) {
  pcg_unique_128_step_r(rng);
  return pcg_output_xsl_rr_128_64(rng->state);
}
#endif

#if PCG_HAS_128BIT_OPS
inline uint64_t
pcg_unique_128_xsl_rr_64_boundedrand_r(struct pcg_state_128 *rng,
                                       uint64_t bound) {
  uint64_t threshold = -bound % bound;
  for (;;) {
    uint64_t r = pcg_unique_128_xsl_rr_64_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}
#endif

inline uint32_t
pcg_setseq_64_xsl_rr_32_random_r(struct pcg_state_setseq_64 *rng) {
  uint64_t oldstate = rng->state;
  pcg_setseq_64_step_r(rng);
  return pcg_output_xsl_rr_64_32(oldstate);
}

inline uint32_t
pcg_setseq_64_xsl_rr_32_boundedrand_r(struct pcg_state_setseq_64 *rng,
                                      uint32_t bound) {
  uint32_t threshold = -bound % bound;
  for (;;) {
    uint32_t r = pcg_setseq_64_xsl_rr_32_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}

#if PCG_HAS_128BIT_OPS
inline uint64_t
pcg_setseq_128_xsl_rr_64_random_r(struct pcg_state_setseq_128 *rng) {
  pcg_setseq_128_step_r(rng);
  return pcg_output_xsl_rr_128_64(rng->state);
}
#endif

#if PCG_HAS_128BIT_OPS
inline uint64_t
pcg_setseq_128_xsl_rr_64_boundedrand_r(struct pcg_state_setseq_128 *rng,
                                       uint64_t bound) {
  uint64_t threshold = -bound % bound;
  for (;;) {
    uint64_t r = pcg_setseq_128_xsl_rr_64_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}
#endif

inline uint32_t pcg_mcg_64_xsl_rr_32_random_r(struct pcg_state_64 *rng) {
  uint64_t oldstate = rng->state;
  pcg_mcg_64_step_r(rng);
  return pcg_output_xsl_rr_64_32(oldstate);
}

inline uint32_t pcg_mcg_64_xsl_rr_32_boundedrand_r(struct pcg_state_64 *rng,
                                                   uint32_t bound) {
  uint32_t threshold = -bound % bound;
  for (;;) {
    uint32_t r = pcg_mcg_64_xsl_rr_32_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}

#if PCG_HAS_128BIT_OPS
inline uint64_t pcg_mcg_128_xsl_rr_64_random_r(struct pcg_state_128 *rng) {
  pcg_mcg_128_step_r(rng);
  return pcg_output_xsl_rr_128_64(rng->state);
}
#endif

#if PCG_HAS_128BIT_OPS
inline uint64_t pcg_mcg_128_xsl_rr_64_boundedrand_r(struct pcg_state_128 *rng,
                                                    uint64_t bound) {
  uint64_t threshold = -bound % bound;
  for (;;) {
    uint64_t r = pcg_mcg_128_xsl_rr_64_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}
#endif

/* Generation functions for XSL RR RR (only defined for "large" types) */

inline uint64_t pcg_oneseq_64_xsl_rr_rr_64_random_r(struct pcg_state_64 *rng) {
  uint64_t oldstate = rng->state;
  pcg_oneseq_64_step_r(rng);
  return pcg_output_xsl_rr_rr_64_64(oldstate);
}

inline uint64_t
pcg_oneseq_64_xsl_rr_rr_64_boundedrand_r(struct pcg_state_64 *rng,
                                         uint64_t bound) {
  uint64_t threshold = -bound % bound;
  for (;;) {
    uint64_t r = pcg_oneseq_64_xsl_rr_rr_64_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}

#if PCG_HAS_128BIT_OPS
inline pcg128_t
pcg_oneseq_128_xsl_rr_rr_128_random_r(struct pcg_state_128 *rng) {
  pcg_oneseq_128_step_r(rng);
  return pcg_output_xsl_rr_rr_128_128(rng->state);
}
#endif

#if PCG_HAS_128BIT_OPS
inline pcg128_t
pcg_oneseq_128_xsl_rr_rr_128_boundedrand_r(struct pcg_state_128 *rng,
                                           pcg128_t bound) {
  pcg128_t threshold = -bound % bound;
  for (;;) {
    pcg128_t r = pcg_oneseq_128_xsl_rr_rr_128_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}
#endif

inline uint64_t pcg_unique_64_xsl_rr_rr_64_random_r(struct pcg_state_64 *rng) {
  uint64_t oldstate = rng->state;
  pcg_unique_64_step_r(rng);
  return pcg_output_xsl_rr_rr_64_64(oldstate);
}

inline uint64_t
pcg_unique_64_xsl_rr_rr_64_boundedrand_r(struct pcg_state_64 *rng,
                                         uint64_t bound) {
  uint64_t threshold = -bound % bound;
  for (;;) {
    uint64_t r = pcg_unique_64_xsl_rr_rr_64_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}

#if PCG_HAS_128BIT_OPS
inline pcg128_t
pcg_unique_128_xsl_rr_rr_128_random_r(struct pcg_state_128 *rng) {
  pcg_unique_128_step_r(rng);
  return pcg_output_xsl_rr_rr_128_128(rng->state);
}
#endif

#if PCG_HAS_128BIT_OPS
inline pcg128_t
pcg_unique_128_xsl_rr_rr_128_boundedrand_r(struct pcg_state_128 *rng,
                                           pcg128_t bound) {
  pcg128_t threshold = -bound % bound;
  for (;;) {
    pcg128_t r = pcg_unique_128_xsl_rr_rr_128_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}
#endif

inline uint64_t
pcg_setseq_64_xsl_rr_rr_64_random_r(struct pcg_state_setseq_64 *rng) {
  uint64_t oldstate = rng->state;
  pcg_setseq_64_step_r(rng);
  return pcg_output_xsl_rr_rr_64_64(oldstate);
}

inline uint64_t
pcg_setseq_64_xsl_rr_rr_64_boundedrand_r(struct pcg_state_setseq_64 *rng,
                                         uint64_t bound) {
  uint64_t threshold = -bound % bound;
  for (;;) {
    uint64_t r = pcg_setseq_64_xsl_rr_rr_64_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}

#if PCG_HAS_128BIT_OPS
inline pcg128_t
pcg_setseq_128_xsl_rr_rr_128_random_r(struct pcg_state_setseq_128 *rng) {
  pcg_setseq_128_step_r(rng);
  return pcg_output_xsl_rr_rr_128_128(rng->state);
}
#endif

#if PCG_HAS_128BIT_OPS
inline pcg128_t
pcg_setseq_128_xsl_rr_rr_128_boundedrand_r(struct pcg_state_setseq_128 *rng,
                                           pcg128_t bound) {
  pcg128_t threshold = -bound % bound;
  for (;;) {
    pcg128_t r = pcg_setseq_128_xsl_rr_rr_128_random_r(rng);
    if (r >= threshold)
      return r % bound;
  }
}
#endif

//// Typedefs
typedef struct pcg_state_setseq_64 pcg32_random_t;
typedef struct pcg_state_64 pcg32s_random_t;
typedef struct pcg_state_64 pcg32u_random_t;
typedef struct pcg_state_64 pcg32f_random_t;
//// random_r
#define pcg32_random_r pcg_setseq_64_xsh_rr_32_random_r
#define pcg32s_random_r pcg_oneseq_64_xsh_rr_32_random_r
#define pcg32u_random_r pcg_unique_64_xsh_rr_32_random_r
#define pcg32f_random_r pcg_mcg_64_xsh_rs_32_random_r
//// boundedrand_r
#define pcg32_boundedrand_r pcg_setseq_64_xsh_rr_32_boundedrand_r
#define pcg32s_boundedrand_r pcg_oneseq_64_xsh_rr_32_boundedrand_r
#define pcg32u_boundedrand_r pcg_unique_64_xsh_rr_32_boundedrand_r
#define pcg32f_boundedrand_r pcg_mcg_64_xsh_rs_32_boundedrand_r
//// srandom_r
#define pcg32_srandom_r pcg_setseq_64_srandom_r
#define pcg32s_srandom_r pcg_oneseq_64_srandom_r
#define pcg32u_srandom_r pcg_unique_64_srandom_r
#define pcg32f_srandom_r pcg_mcg_64_srandom_r
//// advance_r
#define pcg32_advance_r pcg_setseq_64_advance_r
#define pcg32s_advance_r pcg_oneseq_64_advance_r
#define pcg32u_advance_r pcg_unique_64_advance_r
#define pcg32f_advance_r pcg_mcg_64_advance_r

#if PCG_HAS_128BIT_OPS
//// Typedefs
typedef struct pcg_state_setseq_128 pcg64_random_t;
typedef struct pcg_state_128 pcg64s_random_t;
typedef struct pcg_state_128 pcg64u_random_t;
typedef struct pcg_state_128 pcg64f_random_t;
//// random_r
#define pcg64_random_r pcg_setseq_128_xsl_rr_64_random_r
#define pcg64s_random_r pcg_oneseq_128_xsl_rr_64_random_r
#define pcg64u_random_r pcg_unique_128_xsl_rr_64_random_r
#define pcg64f_random_r pcg_mcg_128_xsl_rr_64_random_r
//// boundedrand_r
#define pcg64_boundedrand_r pcg_setseq_128_xsl_rr_64_boundedrand_r
#define pcg64s_boundedrand_r pcg_oneseq_128_xsl_rr_64_boundedrand_r
#define pcg64u_boundedrand_r pcg_unique_128_xsl_rr_64_boundedrand_r
#define pcg64f_boundedrand_r pcg_mcg_128_xsl_rr_64_boundedrand_r
//// srandom_r
#define pcg64_srandom_r pcg_setseq_128_srandom_r
#define pcg64s_srandom_r pcg_oneseq_128_srandom_r
#define pcg64u_srandom_r pcg_unique_128_srandom_r
#define pcg64f_srandom_r pcg_mcg_128_srandom_r
//// advance_r
#define pcg64_advance_r pcg_setseq_128_advance_r
#define pcg64s_advance_r pcg_oneseq_128_advance_r
#define pcg64u_advance_r pcg_unique_128_advance_r
#define pcg64f_advance_r pcg_mcg_128_advance_r
#endif

//// Typedefs
typedef struct pcg_state_8 pcg8si_random_t;
typedef struct pcg_state_16 pcg16si_random_t;
typedef struct pcg_state_32 pcg32si_random_t;
typedef struct pcg_state_64 pcg64si_random_t;
//// random_r
#define pcg8si_random_r pcg_oneseq_8_rxs_m_xs_8_random_r
#define pcg16si_random_r pcg_oneseq_16_rxs_m_xs_16_random_r
#define pcg32si_random_r pcg_oneseq_32_rxs_m_xs_32_random_r
#define pcg64si_random_r pcg_oneseq_64_rxs_m_xs_64_random_r
//// boundedrand_r
#define pcg8si_boundedrand_r pcg_oneseq_8_rxs_m_xs_8_boundedrand_r
#define pcg16si_boundedrand_r pcg_oneseq_16_rxs_m_xs_16_boundedrand_r
#define pcg32si_boundedrand_r pcg_oneseq_32_rxs_m_xs_32_boundedrand_r
#define pcg64si_boundedrand_r pcg_oneseq_64_rxs_m_xs_64_boundedrand_r
//// srandom_r
#define pcg8si_srandom_r pcg_oneseq_8_srandom_r
#define pcg16si_srandom_r pcg_oneseq_16_srandom_r
#define pcg32si_srandom_r pcg_oneseq_32_srandom_r
#define pcg64si_srandom_r pcg_oneseq_64_srandom_r
//// advance_r
#define pcg8si_advance_r pcg_oneseq_8_advance_r
#define pcg16si_advance_r pcg_oneseq_16_advance_r
#define pcg32si_advance_r pcg_oneseq_32_advance_r
#define pcg64si_advance_r pcg_oneseq_64_advance_r

#if PCG_HAS_128BIT_OPS
typedef struct pcg_state_128 pcg128si_random_t;
#define pcg128si_random_r pcg_oneseq_128_rxs_m_xs_128_random_r
#define pcg128si_boundedrand_r pcg_oneseq_128_rxs_m_xs_128_boundedrand_r
#define pcg128si_srandom_r pcg_oneseq_128_srandom_r
#define pcg128si_advance_r pcg_oneseq_128_advance_r
#endif

//// Typedefs
typedef struct pcg_state_setseq_8 pcg8i_random_t;
typedef struct pcg_state_setseq_16 pcg16i_random_t;
typedef struct pcg_state_setseq_32 pcg32i_random_t;
typedef struct pcg_state_setseq_64 pcg64i_random_t;
//// random_r
#define pcg8i_random_r pcg_setseq_8_rxs_m_xs_8_random_r
#define pcg16i_random_r pcg_setseq_16_rxs_m_xs_16_random_r
#define pcg32i_random_r pcg_setseq_32_rxs_m_xs_32_random_r
#define pcg64i_random_r pcg_setseq_64_rxs_m_xs_64_random_r
//// boundedrand_r
#define pcg8i_boundedrand_r pcg_setseq_8_rxs_m_xs_8_boundedrand_r
#define pcg16i_boundedrand_r pcg_setseq_16_rxs_m_xs_16_boundedrand_r
#define pcg32i_boundedrand_r pcg_setseq_32_rxs_m_xs_32_boundedrand_r
#define pcg64i_boundedrand_r pcg_setseq_64_rxs_m_xs_64_boundedrand_r
//// srandom_r
#define pcg8i_srandom_r pcg_setseq_8_srandom_r
#define pcg16i_srandom_r pcg_setseq_16_srandom_r
#define pcg32i_srandom_r pcg_setseq_32_srandom_r
#define pcg64i_srandom_r pcg_setseq_64_srandom_r
//// advance_r
#define pcg8i_advance_r pcg_setseq_8_advance_r
#define pcg16i_advance_r pcg_setseq_16_advance_r
#define pcg32i_advance_r pcg_setseq_32_advance_r
#define pcg64i_advance_r pcg_setseq_64_advance_r

#if PCG_HAS_128BIT_OPS
typedef struct pcg_state_setseq_128 pcg128i_random_t;
#define pcg128i_random_r pcg_setseq_128_rxs_m_xs_128_random_r
#define pcg128i_boundedrand_r pcg_setseq_128_rxs_m_xs_128_boundedrand_r
#define pcg128i_srandom_r pcg_setseq_128_srandom_r
#define pcg128i_advance_r pcg_setseq_128_advance_r
#endif

extern uint32_t pcg32_random();
extern uint32_t pcg32_boundedrand(uint32_t bound);
extern void pcg32_srandom(uint64_t seed, uint64_t seq);
extern void pcg32_advance(uint64_t delta);

#if PCG_HAS_128BIT_OPS
extern uint64_t pcg64_random();
extern uint64_t pcg64_boundedrand(uint64_t bound);
extern void pcg64_srandom(pcg128_t seed, pcg128_t seq);
extern void pcg64_advance(pcg128_t delta);
#endif

/*
 * Static initialization constants (if you can't call srandom for some
 * bizarre reason).
 */

#define PCG32_INITIALIZER PCG_STATE_SETSEQ_64_INITIALIZER
#define PCG32U_INITIALIZER PCG_STATE_UNIQUE_64_INITIALIZER
#define PCG32S_INITIALIZER PCG_STATE_ONESEQ_64_INITIALIZER
#define PCG32F_INITIALIZER PCG_STATE_MCG_64_INITIALIZER

#if PCG_HAS_128BIT_OPS
#define PCG64_INITIALIZER PCG_STATE_SETSEQ_128_INITIALIZER
#define PCG64U_INITIALIZER PCG_STATE_UNIQUE_128_INITIALIZER
#define PCG64S_INITIALIZER PCG_STATE_ONESEQ_128_INITIALIZER
#define PCG64F_INITIALIZER PCG_STATE_MCG_128_INITIALIZER
#endif

#define PCG8SI_INITIALIZER PCG_STATE_ONESEQ_8_INITIALIZER
#define PCG16SI_INITIALIZER PCG_STATE_ONESEQ_16_INITIALIZER
#define PCG32SI_INITIALIZER PCG_STATE_ONESEQ_32_INITIALIZER
#define PCG64SI_INITIALIZER PCG_STATE_ONESEQ_64_INITIALIZER
#if PCG_HAS_128BIT_OPS
#define PCG128SI_INITIALIZER PCG_STATE_ONESEQ_128_INITIALIZER
#endif

#define PCG8I_INITIALIZER PCG_STATE_SETSEQ_8_INITIALIZER
#define PCG16I_INITIALIZER PCG_STATE_SETSEQ_16_INITIALIZER
#define PCG32I_INITIALIZER PCG_STATE_SETSEQ_32_INITIALIZER
#define PCG64I_INITIALIZER PCG_STATE_SETSEQ_64_INITIALIZER
#if PCG_HAS_128BIT_OPS
#define PCG128I_INITIALIZER PCG_STATE_SETSEQ_128_INITIALIZER
#endif

#if __cplusplus
}
#endif

#endif // PCG_VARIANTS_H_INCLUDED
