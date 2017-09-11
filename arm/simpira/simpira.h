/*
Implementation of Simpira-F and Simpira-H for ARM.
*/

#ifndef SIMPIRA_H_
#define SIMPIRA_H_

#include <arm_neon.h>

#define u64 unsigned long
#define u128 uint8x16_t

static const u128 zero = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static const uint32x4_t cB2 = {0x00^2, 0x10^2, 0x20^2, 0x30^2};
static const uint32x4_t cB4 = {0x00^4, 0x10^4, 0x20^4, 0x30^4};

#define C(i, B) (u128)veorq_u32(vdupq_n_u32(i), B)
#define XOR(a, b) veorq_u8(a, b)
#define LOAD(src) vld1q_u8(src)
#define STORE(dest,src) vst1q_u8(dest,src)

// Two rounds of AES with key additions in the end
#define R(x, y, c) { \
  y = XOR(vaesmcq_u8(vaeseq_u8(vaesmcq_u8(vaeseq_u8(x, zero)), c)), y); \
}

#define R4(x0, x1, x2, x3, c0, c1) { \
  x1 = XOR(vaesmcq_u8(vaeseq_u8(vaesmcq_u8(vaeseq_u8(x0, zero)), c0)), x1); \
  x3 = XOR(vaesmcq_u8(vaeseq_u8(vaesmcq_u8(vaeseq_u8(x2, zero)), c1)), x3); \
}

void simpira_testvectors();

void simpira_f(unsigned char *out, const unsigned char *in);
void simpira_f_4x(unsigned char *out, const unsigned char *in);
void simpira_f_8x(unsigned char *out, const unsigned char *in);

void simpira_h(unsigned char *out, const unsigned char *in);
void simpira_h_4x(unsigned char *out, const unsigned char *in);
void simpira_h_8x(unsigned char *out, const unsigned char *in);


#endif
