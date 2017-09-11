#include "keccak.h"
#include "keccak800.h"
#include "KeccakP-800-SnP.h"

#include <arm_neon.h>

void KeccakP800_f(unsigned char *out, const unsigned char *in) {
  ALIGN(8) unsigned static char state[100];
  KeccakP800_Initialize(state);
  KeccakP800_AddBytes(state, in, 0, 32);
  KeccakP800_Permute_12rounds(state);
  KeccakP800_ExtractBytes(state, out, 0, 32);
}

void KeccakP800_f_4x(unsigned char *out, const unsigned char *in) {
  ALIGN(8) unsigned static char state[200*4];
  KeccakP800times4_InitializeAll(state);
  KeccakP800times4_AddLanesAll(state, in, 8, 8);
  KeccakP800times4_PermuteAll_12rounds(state);
  KeccakP800times4_ExtractLanesAll(state, out, 8, 8);
}

void KeccakP800_f_8x(unsigned char *out, const unsigned char *in) {
    KeccakP800_f_4x(out, in);
    KeccakP800_f_4x(out + 128, in + 128);
}

void KeccakP800_h(unsigned char *out, const unsigned char *in) {
    ALIGN(8) unsigned static char state[100];
    KeccakP800_Initialize(state);
    KeccakP800_AddBytes(state, in, 0, 64);
    KeccakP800_Permute_12rounds(state);
    KeccakP800_ExtractBytes(state, out, 0, 32);
}

void KeccakP800_h_4x(unsigned char *out, const unsigned char *in) {
    ALIGN(8) unsigned static char state[200*4];
    KeccakP800times4_InitializeAll(state);
    KeccakP800times4_AddLanesAll(state, in, 16, 16);
    KeccakP800times4_PermuteAll_12rounds(state);
    KeccakP800times4_ExtractLanesAll(state, out, 8, 8);
}

void KeccakP800_h_8x(unsigned char *out, const unsigned char *in) {
    KeccakP800_h_4x(out, in);
    KeccakP800_h_4x(out + 128, in + 256);
}
