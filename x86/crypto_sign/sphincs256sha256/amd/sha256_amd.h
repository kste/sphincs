#ifndef SHA256_AMD_H
#define SHA256_AMD_H

#include <stdint.h>
#include "immintrin.h"

#define u32 uint32_t
#define u128 __m128i
#define LOAD128(src) _mm_loadu_si128((const u128 *)(src))
#define STORE128(dest,src) _mm_storeu_si128((u128 *)(dest),src)
#define ADD128(a, b) _mm_add_epi32(a, b)

static const u32 RC[] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

// static const u32 IV[] = {
//   0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
//   0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
// };

static const char IV[32] = {
  0x6a,0x09,0xe6,0x67,
  0xbb,0x67,0xae,0x85,
  0x3c,0x6e,0xf3,0x72,
  0xa5,0x4f,0xf5,0x3a,
  0x51,0x0e,0x52,0x7f,
  0x9b,0x05,0x68,0x8c,
  0x1f,0x83,0xd9,0xab,
  0x5b,0xe0,0xcd,0x19,
} ;

// Compute 16 rounds of SHA256 on the state s0,s1,s2,s3
#define SHA256_16ROUNDS(i0, i1, i2, i3, c0, c1, c2, c3) \
  h0 = _mm_add_epi32(i0, c0);   \
  x1 = _mm_sha256rnds2_epu32(s1, s0, h0); \
  h0 = _mm_shuffle_epi32(h0, 0x0e); \
  x0 = _mm_sha256rnds2_epu32(s0, x1, h0); \
            \
  h1 = _mm_add_epi32(i1, c1);   \
  s1 = _mm_sha256rnds2_epu32(x1, x0, h1); \
  h1 = _mm_shuffle_epi32(h1, 0x0e); \
  s0 = _mm_sha256rnds2_epu32(x0, s1, h1); \
            \
  h0 = _mm_add_epi32(i2, c2);   \
  x1 = _mm_sha256rnds2_epu32(s1, s0, h0); \
  h0 = _mm_shuffle_epi32(h0, 0x0e); \
  x0 = _mm_sha256rnds2_epu32(s0, x1, h0); \
            \
  h1 = _mm_add_epi32(i3, c3);   \
  s1 = _mm_sha256rnds2_epu32(x1, x0, h1); \
  h1 = _mm_shuffle_epi32(h1, 0x0e); \
  s0 = _mm_sha256rnds2_epu32(x0, s1, h1)

#define SHA256_16EXPANDS(i0, i1, i2, i3, j0, j1, j2, j3) \
  j0 = _mm_sha256msg1_epu32(i0, i1);  \
  h0 = _mm_alignr_epi8(i3, i2, 0x04); \
  j0 = _mm_add_epi32(j0, h0);   \
  j0 = _mm_sha256msg2_epu32(j0, i3);  \
            \
  j1 = _mm_sha256msg1_epu32(i1, i2);  \
  h0 = _mm_alignr_epi8(j0, i3, 0x04); \
  j1 = _mm_add_epi32(j1, h0);   \
  j1 = _mm_sha256msg2_epu32(j1, j0);  \
            \
  j2 = _mm_sha256msg1_epu32(i2, i3);  \
  h0 = _mm_alignr_epi8(j1, j0, 0x04); \
  j2 = _mm_add_epi32(j2, h0);   \
  j2 = _mm_sha256msg2_epu32(j2, j1);  \
            \
  j3 = _mm_sha256msg1_epu32(i3, j0);  \
  h0 = _mm_alignr_epi8(j2, j1, 0x04); \
  j3 = _mm_add_epi32(j3, h0);   \
  j3 = _mm_sha256msg2_epu32(j3, j2)

// Compute 16 rounds of SHA256 on the state s0,s1,s2,s3
#define SHA256_16ROUNDS_PARALLEL(i0, i1, i2, i3, c0, c1, c2, c3, p) \
  h0[p] = _mm_add_epi32(i0, c0);   \
  x1[p] = _mm_sha256rnds2_epu32(s1[p], s0[p], h0[p]); \
  h0[p] = _mm_shuffle_epi32(h0[p], 0x0e); \
  x0[p] = _mm_sha256rnds2_epu32(s0[p], x1[p], h0[p]); \
      \
  h1[p] = _mm_add_epi32(i1, c1);   \
  s1[p] = _mm_sha256rnds2_epu32(x1[p], x0[p], h1[p]); \
  h1[p] = _mm_shuffle_epi32(h1[p], 0x0e); \
  s0[p] = _mm_sha256rnds2_epu32(x0[p], s1[p], h1[p]); \
      \
  h0[p] = _mm_add_epi32(i2, c2);   \
  x1[p] = _mm_sha256rnds2_epu32(s1[p], s0[p], h0[p]); \
  h0[p] = _mm_shuffle_epi32(h0[p], 0x0e); \
  x0[p] = _mm_sha256rnds2_epu32(s0[p], x1[p], h0[p]); \
      \
  h1[p] = _mm_add_epi32(i3, c3);   \
  s1[p] = _mm_sha256rnds2_epu32(x1[p], x0[p], h1[p]); \
  h1[p] = _mm_shuffle_epi32(h1[p], 0x0e); \
  s0[p] = _mm_sha256rnds2_epu32(x0[p], s1[p], h1[p])

#define SHA256_16EXPANDS_PARALLEL(i0, i1, i2, i3, j0, j1, j2, j3, p) \
  j0 = _mm_sha256msg1_epu32(i0, i1);  \
  h0[p] = _mm_alignr_epi8(i3, i2, 0x04); \
  j0 = _mm_add_epi32(j0, h0[p]);   \
  j0 = _mm_sha256msg2_epu32(j0, i3);  \
            \
  j1 = _mm_sha256msg1_epu32(i1, i2);  \
  h0[p] = _mm_alignr_epi8(j0, i3, 0x04); \
  j1 = _mm_add_epi32(j1, h0[p]);   \
  j1 = _mm_sha256msg2_epu32(j1, j0);  \
            \
  j2 = _mm_sha256msg1_epu32(i2, i3);  \
  h0[p] = _mm_alignr_epi8(j1, j0, 0x04); \
  j2 = _mm_add_epi32(j2, h0[p]);   \
  j2 = _mm_sha256msg2_epu32(j2, j1);  \
            \
  j3 = _mm_sha256msg1_epu32(i3, j0);  \
  h0[p] = _mm_alignr_epi8(j2, j1, 0x04); \
  j3 = _mm_add_epi32(j3, h0[p]);   \
  j3 = _mm_sha256msg2_epu32(j3, j2)


void sha256_4x(unsigned char *out, const unsigned char *in);
void sha256_f(unsigned char *out, const unsigned char *in);
void sha256_f_4x(unsigned char *out, const unsigned char *in);
void sha256_h(unsigned char *out, const unsigned char *in);
void sha256_h_4x(unsigned char *out, const unsigned char *in);

#endif
