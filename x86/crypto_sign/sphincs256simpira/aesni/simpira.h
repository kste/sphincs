/*
Optimized Implementations for Simpira256 and Simpira512
*/
#ifndef SIMPIRA_H_
#define SIMPIRA_H_

#include "immintrin.h"

#define u128 __m128i
#define LOAD(src) _mm_load_si128((u128 *)(src))
#define STORE(dest,src) _mm_storeu_si128((u128 *)(dest),src)

#define CB2(i) _mm_setr_epi32(0x00^(i)^2, 0x10^(i)^2, 0x20^(i)^2, 0x30^(i)^2)

#define R(y,z,i, c_0){ \
  ((__m128i*)c_0)[0] = CB2(i); \
  z = _mm_aesenc_si128(_mm_aesenc_si128(y,((__m128i*)c_0)[0]), z); \
}

#define R_Piped(y,z,i, c_0){ \
  z = _mm_aesenc_si128(_mm_aesenc_si128(y,((__m128i*)c_0)[0]), z); \
}

#define CB4_0(i) _mm_setr_epi32(0x00^(2*i+1)^4, 0x10^(2*i+1)^4, 0x20^(2*i+1)^4, 0x30^(2*i+1)^4)
#define CB4_1(i) _mm_setr_epi32(0x00^(2*i+2)^4, 0x10^(2*i+2)^4, 0x20^(2*i+2)^4, 0x30^(2*i+2)^4)

#define R2(w,x,y,z,i) do { \
  x = _mm_aesenc_si128(_mm_aesenc_si128(w,CB4_0(i)), x); \
  z = _mm_aesenc_si128(_mm_aesenc_si128(y,CB4_1(i)), z); \
} while (0)

#define R2_piped(w,x,y,z,c_0, c_1) do { \
  x = _mm_aesenc_si128(_mm_aesenc_si128(w,((__m128i*)c_0)[0]), x); \
  z = _mm_aesenc_si128(_mm_aesenc_si128(y,((__m128i*)c_1)[0]), z); \
} while (0)

#define LOAD(src) _mm_load_si128((u128 *)(src))
#define STORE(dest,src) _mm_storeu_si128((u128 *)(dest),src)
#define XOR(a, b) _mm_xor_si128(a, b)

void simpira_f(unsigned char *out, const unsigned char *in);
void simpira_f_4x(unsigned char *out, const unsigned char *in);
void simpira_f_8x(unsigned char *out, const unsigned char *in);

void simpira_h(unsigned char *out, const unsigned char *in);
void simpira_h_4x(unsigned char *out, const unsigned char *in);
void simpira_h_8x(unsigned char *out, const unsigned char *in);


#endif
