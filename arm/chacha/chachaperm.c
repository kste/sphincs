/*
Optimized ChaCha12 permutation based on the implementation by
D. J. Bernstein
Romain Dolbeau
*/
#include "chachaperm.h"
#include "./e/ecrypt-portable.h"
#include <arm_neon.h>

#define vrotlq_u32(a,b) vsliq_n_u32(vshrq_n_u32(a, 32-b),a,b)

void chacha_f(unsigned char *out,const unsigned char *in) {
  unsigned char x[64];
  int i;

  for(i=0;i<32;i++)
  {
    x[i]    = in[i];
    x[i+32] = hashc[i];
  }
  chacha_perm(x,x);
  for(i=0;i<32;i++)
    out[i] = x[i];
}

void chacha_f_8x(unsigned char *out,const unsigned char *in) {
    unsigned char x[64 * 8];
    int i, j;
    for(i=0;i<32;i++)
    {
        for (j=0;j<8;j++) {
            x[64*j + i] = in[32*j + i];
            x[64*j + i+32] = hashc[i];
        }
    }
    chacha_perm_4x(x, x);
    chacha_perm_4x(x + 128, x + 128);
    for(i=0;i<32*8;i++)
      out[i] = x[i];
}

void chacha_h(unsigned char *out,const unsigned char *in) {
    unsigned char x[64];
    int i;
    for(i=0;i<32;i++)
    {
      x[i]    = in[i];
      x[i+32] = hashc[i];
    }
    chacha_perm(x,x);
    for(i=0;i<32;i++)
      x[i] = x[i] ^ in[i+32];
    chacha_perm(x,x);
    for(i=0;i<32;i++)
      out[i] = x[i];
}

void chacha_h_8x(unsigned char *out,const unsigned char *in) {
    unsigned char x[64 * 8];
    int i, j;
    for(i=0;i<32;i++)
    {
        for (j=0;j<8;j++) {
            x[64*j + i] = in[64*j + i];
            x[64*j + i+32] = hashc[i];
        }
    }
    chacha_perm_4x(out, x);
    chacha_perm_4x(out + 128, x + 128);
    for(i=0;i<32;i++) {
        for(j=0;j<8;j++) {
            x[64*j + i] = x[64*j + i] ^ in[64*j + i + 32];
        }
    }
    chacha_perm_4x(x, x);
    chacha_perm_4x(x + 128, x + 128);
    for(i=0;i<32*8;i++)
      out[i] = x[i];

}

void chacha_perm(unsigned char out[64],const unsigned char in [64]) {
    uint32x4_t x_0, x_1, x_2, x_3;
    uint32x4_t t_1;
    u32* x = (u32*)&in;
    int i;

    x_0 = vld1q_u32(x +  0);
    x_1 = vld1q_u32(x +  4);
    x_2 = vld1q_u32(x +  8);
    x_3 = vld1q_u32(x + 12);

    for (i = 0 ; i < ROUNDS ; i += 2) {
      x_0 = vaddq_u32(x_0, x_1);
      x_3 = veorq_u32(x_3, x_0);
      x_3 = vrotlq_u32(x_3, 16);

      x_2 = vaddq_u32(x_2, x_3);
      x_1 = veorq_u32(x_1, x_2);

/*       t_1 = x_1; */
/*       x_1 = vshlq_n_u32(x_1, 12); */
/*       t_1 = vshrq_n_u32(t_1, 20); */
/*       x_1 = veorq_u32(x_1, t_1); */
      x_1 = vrotlq_u32(x_1, 12);

      x_0 = vaddq_u32(x_0, x_1);
      x_3 = veorq_u32(x_3, x_0);
      x_0 = vextq_u32(x_0, x_0, 3);
      x_3 = vrotlq_u32(x_3, 8);

      x_2 = vaddq_u32(x_2, x_3);
      x_3 = vextq_u32(x_3, x_3, 2);
      x_1 = veorq_u32(x_1, x_2);
      x_2 = vextq_u32(x_2, x_2, 1);

/*       t_1 = x_1; */
/*       x_1 = vshlq_n_u32(x_1, 7); */
/*       t_1 = vshrq_n_u32(t_1, 25); */
/*       x_1 = veorq_u32(x_1, t_1); */
      x_1 = vrotlq_u32(x_1, 7);

      x_0 = vaddq_u32(x_0, x_1);
      x_3 = veorq_u32(x_3, x_0);
      x_3 = vrotlq_u32(x_3, 16);

      x_2 = vaddq_u32(x_2, x_3);
      x_1 = veorq_u32(x_1, x_2);

/*       t_1 = x_1; */
/*       x_1 = vshlq_n_u32(x_1, 12); */
/*       t_1 = vshrq_n_u32(t_1, 20); */
/*       x_1 = veorq_u32(x_1, t_1); */
      x_1 = vrotlq_u32(x_1, 12);

      x_0 = vaddq_u32(x_0, x_1);
      x_3 = veorq_u32(x_3, x_0);
      x_0 = vextq_u32(x_0, x_0, 1);
      x_3 = vrotlq_u32(x_3, 8);

      x_2 = vaddq_u32(x_2, x_3);
      x_3 = vextq_u32(x_3, x_3, 2);
      x_1 = veorq_u32(x_1, x_2);
      x_2 = vextq_u32(x_2, x_2, 3);

/*       t_1 = x_1; */
/*       x_1 = vshlq_n_u32(x_1, 7); */
/*       t_1 = vshrq_n_u32(t_1, 25); */
/*       x_1 = veorq_u32(x_1, t_1); */
      x_1 = vrotlq_u32(x_1, 7);
    }
    x_0 = vaddq_u32(x_0, vld1q_u32(x +  0));
    x_1 = vaddq_u32(x_1, vld1q_u32(x +  4));
    x_2 = vaddq_u32(x_2, vld1q_u32(x +  8));
    x_3 = vaddq_u32(x_3, vld1q_u32(x + 12));
    vst1q_u32((u32*)(out +  0),  x_0);
    vst1q_u32((u32*)(out + 16),  x_1);
    vst1q_u32((u32*)(out + 32),  x_2);
    vst1q_u32((u32*)(out + 48),  x_3);
}

#ifndef USE_3INST_ROT
#define VEC4_ROT(a,imm) vsliq_n_u32(vshrq_n_u32(a, 32-imm),a,imm)
#else
#define VEC4_ROT(a,imm) veorq_u32(vshlq_n_u32(a,imm),vshrq_n_u32(a, 32-imm))
#endif

#define VEC4_ROT16(a) vreinterpretq_u32_u16(vrev32q_u16(vreinterpretq_u16_u32(a)))

#define VEC4_QUARTERROUND(a,b,c,d)                                \
   x_##a = vaddq_u32(x_##a, x_##b); t_##a = veorq_u32(x_##d, x_##a); x_##d = VEC4_ROT16(t_##a); \
   x_##c = vaddq_u32(x_##c, x_##d); t_##c = veorq_u32(x_##b, x_##c); x_##b = VEC4_ROT(t_##c, 12); \
   x_##a = vaddq_u32(x_##a, x_##b); t_##a = veorq_u32(x_##d, x_##a); x_##d = VEC4_ROT(t_##a,  8); \
   x_##c = vaddq_u32(x_##c, x_##d); t_##c = veorq_u32(x_##b, x_##c); x_##b = VEC4_ROT(t_##c,  7)

void chacha_perm_4x(unsigned char *out, const unsigned char *in) {
    u32* x = (u32*)&in;
    int i;

    u32 in12, in13;
     uint32x4_t x_0 = vdupq_n_u32(x[0]);
     uint32x4_t x_1 = vdupq_n_u32(x[1]);
     uint32x4_t x_2 = vdupq_n_u32(x[2]);
     uint32x4_t x_3 = vdupq_n_u32(x[3]);
     uint32x4_t x_4 = vdupq_n_u32(x[4]);
     uint32x4_t x_5 = vdupq_n_u32(x[5]);
     uint32x4_t x_6 = vdupq_n_u32(x[6]);
     uint32x4_t x_7 = vdupq_n_u32(x[7]);
     uint32x4_t x_8 = vdupq_n_u32(x[8]);
     uint32x4_t x_9 = vdupq_n_u32(x[9]);
     uint32x4_t x_10 = vdupq_n_u32(x[10]);
     uint32x4_t x_11 = vdupq_n_u32(x[11]);
     uint32x4_t x_12;// = vdupq_n_u32(x[12]); /* useless */
     uint32x4_t x_13;// = vdupq_n_u32(x[13]); /* useless */
     uint32x4_t x_14 = vdupq_n_u32(x[14]);
     uint32x4_t x_15 = vdupq_n_u32(x[15]);
     uint32x4_t orig0 = x_0;
     uint32x4_t orig1 = x_1;
     uint32x4_t orig2 = x_2;
     uint32x4_t orig3 = x_3;
     uint32x4_t orig4 = x_4;
     uint32x4_t orig5 = x_5;
     uint32x4_t orig6 = x_6;
     uint32x4_t orig7 = x_7;
     uint32x4_t orig8 = x_8;
     uint32x4_t orig9 = x_9;
     uint32x4_t orig10 = x_10;
     uint32x4_t orig11 = x_11;
     uint32x4_t orig12;// = x_12; /* useless */
     uint32x4_t orig13;// = x_13; /* useless */
     uint32x4_t orig14 = x_14;
     uint32x4_t orig15 = x_15;
     uint32x4_t t_0;
     uint32x4_t t_1;
     uint32x4_t t_2;
     uint32x4_t t_3;
     uint32x4_t t_4;
     uint32x4_t t_5;
     uint32x4_t t_6;
     uint32x4_t t_7;
     uint32x4_t t_8;
     uint32x4_t t_9;
     uint32x4_t t_10;
     uint32x4_t t_11;
     uint32x4_t t_12;
     uint32x4_t t_13;
     uint32x4_t t_14;
     uint32x4_t t_15;

     x_0 = orig0;
     x_1 = orig1;
     x_2 = orig2;
     x_3 = orig3;
     x_4 = orig4;
     x_5 = orig5;
     x_6 = orig6;
     x_7 = orig7;
     x_8 = orig8;
     x_9 = orig9;
     x_10 = orig10;
     x_11 = orig11;
     //x_12 = orig12; /* useless */
     //x_13 = orig13; /* useless */
     x_14 = orig14;
     x_15 = orig15;



     const uint64x2_t addv12 = vcombine_u64(vcreate_u64(2),vcreate_u64(3));
     const uint64x2_t addv13 = vcombine_u64(vcreate_u64(0),vcreate_u64(1));
     uint64x2_t t12, t13;
     in12 = x[12];
     in13 = x[13];
     u64 in1213 = ((u64)in12) | (((u64)in13) << 32);
     t12 = vdupq_n_u64(in1213);
     t13 = vdupq_n_u64(in1213);

     x_12 = vreinterpretq_u32_u64(vaddq_u64(addv12, t12));
     x_13 = vreinterpretq_u32_u64(vaddq_u64(addv13, t13));

     uint32x4x2_t t;
     t = vuzpq_u32(x_13,x_12);
     x_12 = t.val[0];
     x_13 = t.val[1];

     orig12 = x_12;
     orig13 = x_13;

     in1213 += 4;

     x[12] = in1213 & 0xFFFFFFFF;
     x[13] = (in1213>>32)&0xFFFFFFFF;

     for (i = 0 ; i < ROUNDS ; i+=2) {
       VEC4_QUARTERROUND( 0, 4, 8,12);
       VEC4_QUARTERROUND( 1, 5, 9,13);
       VEC4_QUARTERROUND( 2, 6,10,14);
       VEC4_QUARTERROUND( 3, 7,11,15);
       VEC4_QUARTERROUND( 0, 5,10,15);
       VEC4_QUARTERROUND( 1, 6,11,12);
       VEC4_QUARTERROUND( 2, 7, 8,13);
       VEC4_QUARTERROUND( 3, 4, 9,14);
     }

 #define ONEQUAD_TRANSPOSE(a,b,c,d)                                      \
     {                                                                   \
       uint32x4x2_t t0dq, t1dq;                                          \
       uint32x4_t t0, t1, t2, t3;                                        \
       x_##a = vaddq_u32(x_##a, orig##a);                                \
       x_##b = vaddq_u32(x_##b, orig##b);                                \
       x_##c = vaddq_u32(x_##c, orig##c);                                \
       x_##d = vaddq_u32(x_##d, orig##d);                                \
       t0dq = vtrnq_u32(x_##a,x_##b);                                      \
       t1dq = vtrnq_u32(x_##c,x_##d);                                      \
       x_##a = vreinterpretq_u32_u64(vcombine_u64(vget_low_u64(vreinterpretq_u64_u32(t0dq.val[0])), vget_low_u64(vreinterpretq_u64_u32(t1dq.val[0])))); \
       x_##b = vreinterpretq_u32_u64(vcombine_u64(vget_low_u64(vreinterpretq_u64_u32(t0dq.val[1])), vget_low_u64(vreinterpretq_u64_u32(t1dq.val[1])))); \
       x_##c = vreinterpretq_u32_u64(vcombine_u64(vget_high_u64(vreinterpretq_u64_u32(t0dq.val[0])), vget_high_u64(vreinterpretq_u64_u32(t1dq.val[0])))); \
       x_##d = vreinterpretq_u32_u64(vcombine_u64(vget_high_u64(vreinterpretq_u64_u32(t0dq.val[1])), vget_high_u64(vreinterpretq_u64_u32(t1dq.val[1])))); \
       vst1q_u32((uint32_t*)(out+0),x_##a);                               \
       vst1q_u32((uint32_t*)(out+64),x_##b);                              \
       vst1q_u32((uint32_t*)(out+128),x_##c);                             \
       vst1q_u32((uint32_t*)(out+192),x_##d);                             \
     }
#define ONEQUAD(a,b,c,d) ONEQUAD_TRANSPOSE(a,b,c,d)
     ONEQUAD(0,1,2,3);
     out+=16;
     ONEQUAD(4,5,6,7);
     out+=16;
     ONEQUAD(8,9,10,11);
     out+=16;
     ONEQUAD(12,13,14,15);
     out-=48;
#undef ONEQUAD
#undef ONEQUAD_TRANSPOSE

}
