#ifndef crypto_hash_blake256_H
#define crypto_hash_blake256_H

#define crypto_hash_blake256_sphlib_BYTES 32

#ifdef __cplusplus
extern "C" {
#endif
extern int crypto_hash_blake256_sphlib(unsigned char *,const unsigned char *,unsigned long long);
#ifdef __cplusplus
}
#endif

#define crypto_hash_blake256 crypto_hash_blake256_sphlib
#define crypto_hash_blake256_BYTES crypto_hash_blake256_sphlib_BYTES
#define crypto_hash_blake256_IMPLEMENTATION "crypto_hash/blake256/sphlib"
#ifndef crypto_hash_blake256_sphlib_VERSION
#define crypto_hash_blake256_sphlib_VERSION "-"
#endif
#define crypto_hash_blake256_VERSION crypto_hash_blake256_sphlib_VERSION

#endif
