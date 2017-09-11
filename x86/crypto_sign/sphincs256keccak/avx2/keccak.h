/*
Implementation of the Keccak Permutation on ARM neon.
*/

#ifndef KECCAK_H_
#define KECCAK_H_

#define KECCAK_ROUNDS 12
#define ALIGN(x) __attribute__ ((aligned(x)))

void KeccakP800_f(unsigned char *out, const unsigned char *in);
void KeccakP800_h(unsigned char *out, const unsigned char *in);

void KeccakP800_f_8x(unsigned char *out, const unsigned char *in);
void KeccakP800_h_8x(unsigned char *out, const unsigned char *in);

void KeccakP1600_12round_f_4x(unsigned char *out, const unsigned char *in);
void KeccakP1600_12round_h_4x(unsigned char *out, const unsigned char *in);

void KeccakP1600_f_4x(unsigned char *out, const unsigned char *in);
void KeccakP1600_h_4x(unsigned char *out, const unsigned char *in);


#endif
