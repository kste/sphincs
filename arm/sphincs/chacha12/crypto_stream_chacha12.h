#ifndef crypto_stream_chacha12_H
#define crypto_stream_chacha12_H

#define crypto_stream_chacha12_KEYBYTES 32
#define crypto_stream_chacha12_NONCEBYTES 8


#ifdef __cplusplus
extern "C" {
#endif
extern int crypto_stream(unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);
extern int crypto_stream_xor(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);
#ifdef __cplusplus
}
#endif

#define crypto_stream_chacha12 crypto_stream
#define crypto_stream_chacha12_xor crypto_stream_xor
#define crypto_stream_chacha12_IMPLEMENTATION "crypto_stream/chacha12/moon/avx2/64"
#ifndef crypto_stream_chacha12_moon_avx2_64_VERSION
#define crypto_stream_chacha12_moon_avx2_64_VERSION "-"
#endif
#define crypto_stream_chacha12_VERSION crypto_stream_chacha12_moon_avx2_64_VERSION

#endif
