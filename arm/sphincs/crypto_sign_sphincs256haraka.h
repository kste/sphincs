#ifndef crypto_sign_sphincs256haraka_H
#define crypto_sign_sphincs256haraka_H

#include "params.h"

#define crypto_sign_sphincs256haraka_aesni_SECRETKEYBYTES (SEED_BYTES + crypto_sign_sphincs256haraka_aesni_PUBLICKEYBYTES-HASH_BYTES + SK_RAND_SEED_BYTES)
#define crypto_sign_sphincs256haraka_aesni_PUBLICKEYBYTES ((N_MASKS+1)*HASH_BYTES)
#define crypto_sign_sphincs256haraka_aesni_BYTES (MESSAGE_HASH_SEED_BYTES + (TOTALTREE_HEIGHT+7)/8 + HORST_SIGBYTES + (TOTALTREE_HEIGHT/SUBTREE_HEIGHT)*WOTS_SIGBYTES + TOTALTREE_HEIGHT*HASH_BYTES)
#define crypto_sign_sphincs256haraka_aesni_DETERMINISTIC 1
 
#ifdef __cplusplus
extern "C" {
#endif
extern int crypto_sign_sphincs256haraka_aesni(unsigned char *,unsigned long long *,const unsigned char *,unsigned long long,const unsigned char *);
extern int crypto_sign_sphincs256haraka_aesni_open(unsigned char *,unsigned long long *,const unsigned char *,unsigned long long,const unsigned char *);
extern int crypto_sign_sphincs256haraka_aesni_keypair(unsigned char *,unsigned char *);
#ifdef __cplusplus
}
#endif

#define crypto_sign_sphincs256haraka crypto_sign_sphincs256haraka_aesni
#define crypto_sign_sphincs256haraka_open crypto_sign_sphincs256haraka_aesni_open
#define crypto_sign_sphincs256haraka_keypair crypto_sign_sphincs256haraka_aesni_keypair
#define crypto_sign_sphincs256haraka_BYTES crypto_sign_sphincs256haraka_aesni_BYTES
#define crypto_sign_sphincs256haraka_SECRETKEYBYTES crypto_sign_sphincs256haraka_aesni_SECRETKEYBYTES
#define crypto_sign_sphincs256haraka_PUBLICKEYBYTES crypto_sign_sphincs256haraka_aesni_PUBLICKEYBYTES
#define crypto_sign_sphincs256haraka_DETERMINISTIC crypto_sign_sphincs256haraka_aesni_DETERMINISTIC
#define crypto_sign_sphincs256haraka_IMPLEMENTATION "crypto_sign/sphincs256haraka/aesni"
#ifndef crypto_sign_sphincs256haraka_aesni_VERSION
#define crypto_sign_sphincs256haraka_aesni_VERSION "-"
#endif
#define crypto_sign_sphincs256haraka_VERSION crypto_sign_sphincs256haraka_aesni_VERSION

#endif
