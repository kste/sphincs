#include "params.h"
#include "permute.h"
#include "../crypto_hash_blake256.h"
#include "../crypto_hash_blake512.h"
#include "hash.h"

#include <stddef.h>
#include <arm_neon.h>

#define u128 uint8x16_t
#define XOR(a, b) veorq_u8(a, b)
#define LOAD(src) vld1q_u8(src)
#define STORE(dest,src) vst1q_u8(dest,src)

int varlen_hash(unsigned char *out,const unsigned char *in,unsigned long long inlen)
{
  crypto_hash_blake256(out,in,inlen);
  return 0;
}

int msg_hash(unsigned char *out,const unsigned char *in,unsigned long long inlen)
{
  crypto_hash_blake512(out,in,inlen);
  return 0;
}

int hash_2n_n(unsigned char *out,const unsigned char *in)
{
  hash_h(out, in);
  return 0;
}

int hash_2n_n_mask(unsigned char *out,const unsigned char *in, const unsigned char *mask)
{
  unsigned char buf[2*HASH_BYTES];
  int i;
  for(i=0;i<2*HASH_BYTES;i++)
    buf[i] = in[i] ^ mask[i];

  hash_h(out, buf);
  return 0;
}

int hash_n_n(unsigned char *out,const unsigned char *in)
{
  unsigned char x[32];
  int i;

  for(i=0;i<32;i++)
  {
    x[i]    = in[i];
  }
  hash_f(out,x);

  return 0;
}

int hash_n_n_mask(unsigned char *out,const unsigned char *in, const unsigned char *mask)
{
  unsigned char buf[HASH_BYTES];
  int i;
  for(i=0;i<HASH_BYTES;i++)
    buf[i] = in[i] ^ mask[i];

  hash_f(out, buf);
  return 0;
}

int hash_n_n_mask_8x(unsigned char *out,const unsigned char *in,
                     const unsigned char *mask)
{
  unsigned char x[32*8];
  int i;

  u128 fullmask = LOAD(mask);

  for(i=0;i<16;i++)
  {
    STORE(x + 16*i, XOR(LOAD(in + 16*i), fullmask));
  }

  hash_f_8x(out, x);
}

int hash_2n_n_8x(unsigned char *out,const unsigned char *in,
      unsigned long long out_dist, unsigned long long in_dist)
{
  hash_h_8x(out, in);
}
int hash_2n_n_mask_8x(unsigned char *out,const unsigned char *in,
      unsigned long long out_dist, unsigned long long in_dist,
      const unsigned char *mask)
{
  unsigned char x[in_dist*8];
  int i;

  u128 mask_a = LOAD(mask);
  u128 mask_b = LOAD(mask + 16);
  u128 mask_c = LOAD(mask + 32);
  u128 mask_d = LOAD(mask + 48);

  for(i=0;i<8;i++)
  {
    STORE(x + 64*i, XOR(LOAD(in + 64*i), mask_a));
    STORE(x + 64*i + 16, XOR(LOAD(in + 64*i + 16), mask_b));
    STORE(x + 64*i + 32, XOR(LOAD(in + 64*i + 32), mask_c));
    STORE(x + 64*i + 48, XOR(LOAD(in + 64*i + 48), mask_d));
  }
  hash_h_8x(out, x);
}

int loop_hash_2n_n_mask_8x(unsigned char *out,const unsigned char *in,
                           unsigned long loops, const unsigned char *mask)
{
  int j;
  for(j=0;j<8*loops;j+=8)
    hash_2n_n_mask_8x(out+(j)*HASH_BYTES, in+(2*j)*HASH_BYTES,
                      HASH_BYTES, 2*HASH_BYTES, mask);
}

int hash_n_n_8x(unsigned char *out,const unsigned char *in)
{
  hash_f_8x(out, in);
}
