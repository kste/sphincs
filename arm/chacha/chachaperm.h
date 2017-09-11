#ifndef CHACHAPERM_H
#define CHACHAPERM_H

#define ROUNDS 12
#define USE_3INST_ROT

static const char *hashc = "expand 32-byte to 64-byte state!";

void chacha_perm(unsigned char out[64],const unsigned char in[64]);
void chacha_perm_4x(unsigned char *out,const unsigned char *in);

void chacha_f(unsigned char *out,const unsigned char *in);
void chacha_f_8x(unsigned char *out,const unsigned char *in);
void chacha_h(unsigned char *out,const unsigned char *in);
void chacha_h_8x(unsigned char *out,const unsigned char *in);

#endif
