#include "params.h"
#include "immintrin.h"
#include "crypto_hash_blake256.h"
#include "crypto_hash_blake512.h"
#include "sha256.h"

#include <stddef.h>
#include <string.h>

typedef __m256i u256;


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


static const char *hashc = "expand 32-byte to 64-byte state!";

int hash_2n_n(unsigned char *out,const unsigned char *in)
{
  unsigned char x[64];
  int i;
  for(i=0;i<64;i++)
  {
    x[i] = in[i];
  }  
  sha256_h(out,in);
  return 0;
}

int hash_2n_n_mask(unsigned char *out,const unsigned char *in, const unsigned char *mask)
{
  unsigned char buf[2*HASH_BYTES];
  int i;
  for(i=0;i<2*HASH_BYTES;i++)
    buf[i] = in[i] ^ mask[i];
  return hash_2n_n(out, buf);
}

int hash_n_n(unsigned char *out,const unsigned char *in)
{
  unsigned char x[32];
  int i;
  for(i=0;i<32;i++)
  {
    x[i] = in[i];
  }  
  sha256_f(out,in);

  return 0;
}

int hash_n_n_mask(unsigned char *out,const unsigned char *in, const unsigned char *mask)
{
  unsigned char buf[HASH_BYTES];
  int i;
  for(i=0;i<HASH_BYTES;i++)
    buf[i] = in[i] ^ mask[i];
  return hash_n_n(out, buf);
}

int hash_n_n_mask_8x(unsigned char *out,const unsigned char *in,
                     const unsigned char *mask)
{
  unsigned char x[32*8];
  int i;

  __m256i fullmask = _mm256_loadu_si256(mask);

  for(i=0;i<8;i++)
  {
    _mm256_store_si256(x + 32*i, _mm256_xor_si256(_mm256_load_si256(in + 32*i), fullmask));
  }

  sha256_f_8x(out, in);
}

int hash_2n_n_8x(unsigned char *out,const unsigned char *in,
      unsigned long long out_dist, unsigned long long in_dist)
{
  sha256_h_8x(out, in);
}
int hash_2n_n_mask_8x(unsigned char *out,const unsigned char *in,
      unsigned long long out_dist, unsigned long long in_dist,
      const unsigned char *mask)
{
  unsigned char x[in_dist*8];
  int i;

  __m256i mask_a = _mm256_loadu_si256(mask);
  __m256i mask_b = _mm256_loadu_si256(mask + 32);

  for(i=0;i<8;i++)
  {
    _mm256_store_si256(x + 64*i, _mm256_xor_si256(_mm256_load_si256(in + 64*i), mask_a));
    _mm256_store_si256(x + 64*i + 32, _mm256_xor_si256(_mm256_load_si256(in + 64*i + 32), mask_b));
  }
  sha256_h_8x(out, x);
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
  sha256_f_8x(out, in);
}
