#include "keccak.h"
#include "KeccakP-800-times8-SnP.h"
#include "KeccakP-1600-times4-SnP.h"
#include <string.h>

void KeccakP800_f(unsigned char *out, const unsigned char *in) {
  unsigned static char state[100*8];
  unsigned char x[32 * 8], y[32 * 8];

  memset(x, 0, 32 * 4);
  memcpy(x, in, 32);

  KeccakP800times8_InitializeAll(state);
  KeccakP800times8_AddLanesAll(state, x, 16, 16);
  KeccakP800times8_PermuteAll_12rounds(state);
  KeccakP800times8_ExtractLanesAll(state, y, 8, 8);
  memcpy(out, y, 32);
}

void KeccakP800_h(unsigned char *out, const unsigned char *in) {
  unsigned static char state[100*8];
  unsigned char x[32 * 8], y[32 * 8];

  memset(x, 0, 32 * 8);
  memset(y, 0, 32 * 8);
  memcpy(x, in, 64);

  KeccakP800times8_InitializeAll(state);
  KeccakP800times8_AddLanesAll(state, x, 16, 16);
  KeccakP800times8_PermuteAll_12rounds(state);
  KeccakP800times8_ExtractLanesAll(state, y, 8, 8);
  memcpy(out, y, 32);
}


void KeccakP800_f_8x(unsigned char *out, const unsigned char *in) {
  unsigned static char state[100*8];
  KeccakP800times8_InitializeAll(state);
  KeccakP800times8_AddLanesAll(state, in, 8, 8);
  KeccakP800times8_PermuteAll_12rounds(state);
  KeccakP800times8_ExtractLanesAll(state, out, 8, 8);
}

void KeccakP800_h_8x(unsigned char *out, const unsigned char *in) {
  unsigned static char state[100*8];
  KeccakP800times8_InitializeAll(state);
  KeccakP800times8_AddLanesAll(state, in, 16, 16);
  KeccakP800times8_PermuteAll_12rounds(state);
  KeccakP800times8_ExtractLanesAll(state, out, 8, 8);
}

void KeccakP1600_12round_f_4x(unsigned char *out, const unsigned char *in) {
  unsigned static char state[200*4];
  KeccakP1600times4_InitializeAll(state);
  KeccakP1600times4_AddLanesAll(state, in, 4, 4);
  KeccakP1600times4_PermuteAll_12rounds(state);
  KeccakP1600times4_ExtractLanesAll(state, out, 4, 4);
}

void KeccakP1600_12round_h_4x(unsigned char *out, const unsigned char *in) {
  unsigned static char state[200*4];
  KeccakP1600times4_InitializeAll(state);
  KeccakP1600times4_AddLanesAll(state, in, 8, 8);
  KeccakP1600times4_PermuteAll_12rounds(state);
  KeccakP1600times4_ExtractLanesAll(state, out, 4, 4);
}

void KeccakP1600_f_4x(unsigned char *out, const unsigned char *in) {
  unsigned static char state[200*4];
  KeccakP1600times4_InitializeAll(state);
  KeccakP1600times4_AddLanesAll(state, in, 4, 4);
  KeccakP1600times4_PermuteAll_24rounds(state);
  KeccakP1600times4_ExtractLanesAll(state, out, 4, 4);
}

void KeccakP1600_h_4x(unsigned char *out, const unsigned char *in) {
  unsigned static char state[200*4];
  KeccakP1600times4_InitializeAll(state);
  KeccakP1600times4_AddLanesAll(state, in, 8, 8);
  KeccakP1600times4_PermuteAll_24rounds(state);
  KeccakP1600times4_ExtractLanesAll(state, out, 4, 4);
}
