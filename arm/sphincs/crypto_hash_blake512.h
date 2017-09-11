#ifndef crypto_hash_blake512_H
#define crypto_hash_blake512_H

#define crypto_hash_blake512_sphlib_BYTES 64


#ifdef __cplusplus
extern "C" {
#endif
extern int crypto_hash_blake512_sphlib(unsigned char *,const unsigned char *,unsigned long long);
#ifdef __cplusplus
}
#endif

#define crypto_hash_blake512 crypto_hash_blake512_sphlib
#define crypto_hash_blake512_BYTES crypto_hash_blake512_sphlib_BYTES
#define crypto_hash_blake512_IMPLEMENTATION "crypto_hash/blake512/sphlib"
#ifndef crypto_hash_blake512_sphlib_VERSION
#define crypto_hash_blake512_sphlib_VERSION "-"
#endif
#define crypto_hash_blake512_VERSION crypto_hash_blake512_sphlib_VERSION

#endif
