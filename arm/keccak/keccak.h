/*
Implementation of the Keccak Permutation on ARM neon.
*/

#ifndef KECCAK_H_
#define KECCAK_H_

#include "keccak.h"

#define KECCAK_ROUNDS 12
#define ALIGN(x) __attribute__ ((aligned(x)))

void KeccakP400(unsigned char *out, const unsigned char *in);
void KeccakP400_8x(unsigned char *out, const unsigned char *in);

void KeccakP800_f(unsigned char *out, const unsigned char *in);
void KeccakP800_f_4x(unsigned char *out, const unsigned char *in);
void KeccakP800_f_8x(unsigned char *out, const unsigned char *in);
void KeccakP800_h(unsigned char *out, const unsigned char *in);
void KeccakP800_h_4x(unsigned char *out, const unsigned char *in);
void KeccakP800_h_8x(unsigned char *out, const unsigned char *in);

#endif
