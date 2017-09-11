#ifndef HASH_H
#define HASH_H

#include "params.h"
#include "../chacha/chachaperm.h"
#include "../haraka/haraka.h"
#include "../sha256/sha256.h"
#include "../simpira/simpira.h"
#include "../keccak/keccak.h"

// ChaCha as default
#define hash_f chacha_f
#define hash_h chacha_h
#define hash_f_8x chacha_f_8x
#define hash_h_8x chacha_h_8x

#ifdef HARAKA
    #define hash_f haraka_f
    #define hash_h haraka_h
    #define hash_f_8x haraka_f_8x
    #define hash_h_8x haraka_h_8x
#endif
#ifdef KECCAK
    #define hash_f KeccakP800_f
    #define hash_h KeccakP800_h
    #define hash_f_8x KeccakP800_f_8x
    #define hash_h_8x KeccakP800_h_8x
#endif
#ifdef SIMPIRA
    #define hash_f simpira_f
    #define hash_h simpira_h
    #define hash_f_8x simpira_f_8x
    #define hash_h_8x simpira_h_8x
#endif
#ifdef SHA256
    #define hash_f sha256_f
    #define hash_h sha256_h
    #define hash_f_8x sha256_f_8x
    #define hash_h_8x sha256_h_8x
#endif


int msg_hash(unsigned char *out,const unsigned char *in,unsigned long long inlen);
int varlen_hash(unsigned char *out,const unsigned char *in,unsigned long long inlen);
int hash_2n_n(unsigned char *out,const unsigned char *in);
int hash_2n_n_mask(unsigned char *out,const unsigned char *in,const unsigned char *mask);
int hash_n_n(unsigned char *out,const unsigned char *in);
int hash_n_n_mask(unsigned char *out,const unsigned char *in,const unsigned char *mask);

int hash_2n_n_8x(unsigned char *out,const unsigned char *in,
      unsigned long long out_dist, unsigned long long in_dist);
int hash_2n_n_mask_8x(unsigned char *out,const unsigned char *in,
      unsigned long long out_dist, unsigned long long in_dist,
      const unsigned char *mask);
int hash_n_n_8x(unsigned char *out,const unsigned char *in);
void genfullchain8x(unsigned char *out, const unsigned char *masks);

int hash_n_n_mask_8x(unsigned char *out,const unsigned char *in,
                     const unsigned char *mask);

int loop_hash_2n_n_mask_8x(unsigned char *out,const unsigned char *in,
                           unsigned long loops, const unsigned char *mask);

#endif
