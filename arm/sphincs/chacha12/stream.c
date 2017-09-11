#include "crypto_stream_chacha12.h"
#include "e/ecrypt-sync.h"
#include "api.h"

static int flaginitialized = 0;

int crypto_stream(
  unsigned char *c,unsigned long long clen,
  const unsigned char *n,
  const unsigned char *k
)
{
#ifdef ECRYPT_GENERATES_KEYSTREAM
  ECRYPT_ctx ctx;
  if (!flaginitialized) { ECRYPT_init(); flaginitialized = 1; }
  ECRYPT_keysetup(&ctx,k,crypto_stream_chacha12_KEYBYTES * 8,crypto_stream_chacha12_NONCEBYTES * 8);
  ECRYPT_ivsetup(&ctx,n);
  while (clen > 65536) {
    ECRYPT_keystream_bytes(&ctx,c,65536);
    c += 65536; clen -= 65536;
  }
  ECRYPT_keystream_bytes(&ctx,c,clen);
  return 0;
#else
  ECRYPT_ctx ctx;
  unsigned long long i;
  if (!flaginitialized) { ECRYPT_init(); flaginitialized = 1; }
  ECRYPT_keysetup(&ctx,k,crypto_stream_chacha12_KEYBYTES * 8,crypto_stream_chacha12_NONCEBYTES * 8);
  ECRYPT_ivsetup(&ctx,n);
  for (i = 0;i < clen;++i) c[i] = 0;
  while (clen > 65536) {
    ECRYPT_encrypt_bytes(&ctx,c,c,65536);
    c += 65536; clen -= 65536;
  }
  ECRYPT_encrypt_bytes(&ctx,c,c,clen);
  return 0;
#endif
}

int crypto_stream_xor(
  unsigned char *c,
  const unsigned char *m,unsigned long long mlen,
  const unsigned char *n,
  const unsigned char *k
)
{
  ECRYPT_ctx ctx;
  if (!flaginitialized) { ECRYPT_init(); flaginitialized = 1; }
  ECRYPT_keysetup(&ctx,k,crypto_stream_chacha12_KEYBYTES * 8,crypto_stream_chacha12_NONCEBYTES * 8);
  ECRYPT_ivsetup(&ctx,n);
  while (mlen > 65536) {
    ECRYPT_encrypt_bytes(&ctx,m,c,65536);
    m += 65536; c += 65536; mlen -= 65536;
  }
  ECRYPT_encrypt_bytes(&ctx,m,c,mlen);
  return 0;
}
