#include "sha256_amd.h"
#include <string.h>

void sha256_f(unsigned char *out, const unsigned char *in) {
    unsigned char x[64];
    int i;

    for(i=0;i<32;i++)
    {
      x[i]    = in[i];
      x[i+32] = 0;
    }
    sha256(out, x);
}

void sha256_h(unsigned char *out, const unsigned char *in) {
    sha256(out, in);
}

void sha256_f_4x(unsigned char *out, const unsigned char *in) {
    unsigned char x[64*4];

    // We have to pad with 0 for full SHA-256 block.
    memset(x, 0, 64*4);

    int i;
    for(i=0;i<4;i++) {
      STORE128(x + 64*i, LOAD128(in + 32*i));
    }

    sha256_4x(out, in);
}

void sha256_h_4x(unsigned char *out, const unsigned char *in) {
    sha256_4x(out, in);
}

void sha256(unsigned char *out, const unsigned char *in) {
    u32 s[8], w[64], T0, T1;

    // Load constants
    u128 c0 = LOAD128(RC +  0);
    u128 c1 = LOAD128(RC +  4);
    u128 c2 = LOAD128(RC +  8);
    u128 c3 = LOAD128(RC + 12);
    u128 c4 = LOAD128(RC + 16);
    u128 c5 = LOAD128(RC + 20);
    u128 c6 = LOAD128(RC + 24);
    u128 c7 = LOAD128(RC + 28);
    u128 c8 = LOAD128(RC + 32);
    u128 c9 = LOAD128(RC + 36);
    u128 ca = LOAD128(RC + 40);
    u128 cb = LOAD128(RC + 44);
    u128 cc = LOAD128(RC + 48);
    u128 cd = LOAD128(RC + 52);
    u128 ce = LOAD128(RC + 56);
    u128 cf = LOAD128(RC + 60);

    // Load IV into state
    u128 d0 = LOAD128(IV);
    u128 d1 = LOAD128(IV + 4);
    u128 s0, s1, h0, h1;

    // Load message block
    u128 i0 = LOAD128(in);
    u128 i1 = LOAD128(in + 16);
    u128 i2 = LOAD128(in + 32);
    u128 i3 = LOAD128(in + 48);
    u128 j0, j1, j2, j3;
    u128 x0, x1;

    // Copy state
    s0 = d0;
    s1 = d1;

    SHA256_16ROUNDS(i0, i1, i2, i3, c0, c1, c2, c3);

    SHA256_16EXPANDS(i0, i1, i2, i3, j0, j1, j2, j3);

    SHA256_16ROUNDS(j0, j1, j2, j3, c4, c5, c6, c7);

    SHA256_16EXPANDS(j0, j1, j2, j3, i0, i1, i2, i3);

    SHA256_16ROUNDS(i0, i1, i2, i3, c8, c9, ca, cb);

    SHA256_16EXPANDS(i0, i1, i2, i3, j0, j1, j2, j3);

    SHA256_16ROUNDS(j0, j1, j2, j3, cc, cd, ce, cf);

    d0 = ADD128(s0, d0);
    d1 = ADD128(s1, d1);

    // Store Hash value
    STORE128(out, d0);
    STORE128(out + 16, d1);
    return;
}

void sha256_4x(unsigned char *out, const unsigned char *in) {
  u32 s[4][8], w[4][64], T0[4], T1[4];

  // Load constants
  u128 c0 = LOAD128(RC +  0);
  u128 c1 = LOAD128(RC +  4);
  u128 c2 = LOAD128(RC +  8);
  u128 c3 = LOAD128(RC + 12);
  u128 c4 = LOAD128(RC + 16);
  u128 c5 = LOAD128(RC + 20);
  u128 c6 = LOAD128(RC + 24);
  u128 c7 = LOAD128(RC + 28);
  u128 c8 = LOAD128(RC + 32);
  u128 c9 = LOAD128(RC + 36);
  u128 ca = LOAD128(RC + 40);
  u128 cb = LOAD128(RC + 44);
  u128 cc = LOAD128(RC + 48);
  u128 cd = LOAD128(RC + 52);
  u128 ce = LOAD128(RC + 56);
  u128 cf = LOAD128(RC + 60);
  // Load IV into state
  u128 d0[4], d1[4];
  u128 s0[4], s1[4], h0[4], h1[4];
  d0[0] = LOAD128(IV);
  d1[0] = LOAD128(IV + 4);
  d0[1] = LOAD128(IV);
  d1[1] = LOAD128(IV + 4);
  d0[2] = LOAD128(IV);
  d1[2] = LOAD128(IV + 4);
  d0[3] = LOAD128(IV);
  d1[3] = LOAD128(IV + 4);
  u128 i0[4], i1[4], i2[4], i3[4], j0[4], j1[4], j2[4], j3[4], x0[4], x1[4];
  // Load message block
  i0[0] = LOAD128(in + 0);
  i1[0] = LOAD128(in + 16);
  i2[0] = LOAD128(in + 32);
  i3[0] = LOAD128(in + 48);
  i0[1] = LOAD128(in + 64);
  i1[1] = LOAD128(in + 80);
  i2[1] = LOAD128(in + 96);
  i3[1] = LOAD128(in + 112);
  i0[2] = LOAD128(in + 128);
  i1[2] = LOAD128(in + 144);
  i2[2] = LOAD128(in + 160);
  i3[2] = LOAD128(in + 176);
  i0[3] = LOAD128(in + 192);
  i1[3] = LOAD128(in + 208);
  i2[3] = LOAD128(in + 224);
  i3[3] = LOAD128(in + 240);
  // Copy state
  s0[0] = d0[0];
  s1[0] = d1[0];
  s0[1] = d0[1];
  s1[1] = d1[1];
  s0[2] = d0[2];
  s1[2] = d1[2];
  s0[3] = d0[3];
  s1[3] = d1[3];
  SHA256_16ROUNDS_PARALLEL(i0[0], i1[0], i2[0], i3[0], c0, c1, c2, c3, 0);
  SHA256_16ROUNDS_PARALLEL(i0[1], i1[1], i2[1], i3[1], c0, c1, c2, c3, 1);
  SHA256_16ROUNDS_PARALLEL(i0[2], i1[2], i2[2], i3[2], c0, c1, c2, c3, 2);
  SHA256_16ROUNDS_PARALLEL(i0[3], i1[3], i2[3], i3[3], c0, c1, c2, c3, 3);
  SHA256_16EXPANDS_PARALLEL(i0[0], i1[0], i2[0], i3[0], j0[0], j1[0], j2[0], j3[0], 0);
  SHA256_16EXPANDS_PARALLEL(i0[1], i1[1], i2[1], i3[1], j0[1], j1[1], j2[1], j3[1], 1);
  SHA256_16EXPANDS_PARALLEL(i0[2], i1[2], i2[2], i3[2], j0[2], j1[2], j2[2], j3[2], 2);
  SHA256_16EXPANDS_PARALLEL(i0[3], i1[3], i2[3], i3[3], j0[3], j1[3], j2[3], j3[3], 3);
  SHA256_16ROUNDS_PARALLEL(j0[0], j1[0], j2[0], j3[0], c4, c5, c6, c7, 0);
  SHA256_16ROUNDS_PARALLEL(j0[1], j1[1], j2[1], j3[1], c4, c5, c6, c7, 1);
  SHA256_16ROUNDS_PARALLEL(j0[2], j1[2], j2[2], j3[2], c4, c5, c6, c7, 2);
  SHA256_16ROUNDS_PARALLEL(j0[3], j1[3], j2[3], j3[3], c4, c5, c6, c7, 3);
  SHA256_16EXPANDS_PARALLEL(j0[0], j1[0], j2[0], j3[0], i0[0], i1[0], i2[0], i3[0], 0);
  SHA256_16EXPANDS_PARALLEL(j0[1], j1[1], j2[1], j3[1], i0[1], i1[1], i2[1], i3[1], 1);
  SHA256_16EXPANDS_PARALLEL(j0[2], j1[2], j2[2], j3[2], i0[2], i1[2], i2[2], i3[2], 2);
  SHA256_16EXPANDS_PARALLEL(j0[3], j1[3], j2[3], j3[3], i0[3], i1[3], i2[3], i3[3], 3);
  SHA256_16ROUNDS_PARALLEL(i0[0], i1[0], i2[0], i3[0], c8, c9, ca, cb, 0);
  SHA256_16ROUNDS_PARALLEL(i0[1], i1[1], i2[1], i3[1], c8, c9, ca, cb, 1);
  SHA256_16ROUNDS_PARALLEL(i0[2], i1[2], i2[2], i3[2], c8, c9, ca, cb, 2);
  SHA256_16ROUNDS_PARALLEL(i0[3], i1[3], i2[3], i3[3], c8, c9, ca, cb, 3);
  SHA256_16EXPANDS_PARALLEL(i0[0], i1[0], i2[0], i3[0], j0[0], j1[0], j2[0], j3[0], 0);
  SHA256_16EXPANDS_PARALLEL(i0[1], i1[1], i2[1], i3[1], j0[1], j1[1], j2[1], j3[1], 1);
  SHA256_16EXPANDS_PARALLEL(i0[2], i1[2], i2[2], i3[2], j0[2], j1[2], j2[2], j3[2], 2);
  SHA256_16EXPANDS_PARALLEL(i0[3], i1[3], i2[3], i3[3], j0[3], j1[3], j2[3], j3[3], 3);
  SHA256_16ROUNDS_PARALLEL(j0[0], j1[0], j2[0], j3[0], cc, cd, ce, cf, 0);
  SHA256_16ROUNDS_PARALLEL(j0[1], j1[1], j2[1], j3[1], cc, cd, ce, cf, 1);
  SHA256_16ROUNDS_PARALLEL(j0[2], j1[2], j2[2], j3[2], cc, cd, ce, cf, 2);
  SHA256_16ROUNDS_PARALLEL(j0[3], j1[3], j2[3], j3[3], cc, cd, ce, cf, 3);
  d0[0] = ADD128(s0[0], d0[0]);
  d1[0] = ADD128(s1[0], d1[0]);
  d0[1] = ADD128(s0[1], d0[1]);
  d1[1] = ADD128(s1[1], d1[1]);
  d0[2] = ADD128(s0[2], d0[2]);
  d1[2] = ADD128(s1[2], d1[2]);
  d0[3] = ADD128(s0[3], d0[3]);
  d1[3] = ADD128(s1[3], d1[3]);
  STORE128(out + 0, d0[0]);
  STORE128(out + 16, d1[0]);
  STORE128(out + 32, d0[1]);
  STORE128(out + 48, d1[1]);
  STORE128(out + 64, d0[2]);
  STORE128(out + 80, d1[2]);
  STORE128(out + 96, d0[3]);
  STORE128(out + 112, d1[3]);
}
