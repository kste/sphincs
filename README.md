# Putting Wings on SPHINCS
[SPHINCS](https://sphincs.cr.yp.to/) is a recently proposed stateless hash-based signature scheme and
promising candidate for a post-quantum secure digital signature scheme. This repository contains 
optimized implementations of the cryptographic hash functions for both ARM and x86 which are used
to instantiate SPHINCS with (for more details see the [paper](linktobeadded)):

* ChaCha12: The original proposal for SPHINCS.
* [Haraka](https://github.com/kste/haraka): An AES-based short-input hash function.
* [Keccak](https://keccak.team/index.html): Using the 800-bit permutation and 12 rounds.
* SHA256: The SHA256 standard.
* [Simpira](https://mouha.be/simpira/): An AES-based permutation.

All implementations utilize the available crypto extensions on the current AMD, ARM and Intel platforms.

The number of cycles to generate one signature are

Primitive | Intel Haswell | Intel Skylake | AMD Ryzen | ARM Cortex A57 | ARM Cortex A72
------------ | ------------- | ------------- | ------------- | ------------- | -------------
ChaCha12 | 52.249.518 | 43.495.454 | 63.427.980 | 193.512.960 | 199.582.208
Haraka | 33.640.796 | 20.782.894 | 15.545.370 | 47.100.928 | 45.261.312
Keccak[b=800, r=12] | 122.517.136 | 108.629.952 | 189.986.970 | 376.908.288 | 392.445.952
SHA256 | 157.270.152 | 142.063.840 | 53.332.380 | 92.088.832 | 92.767.744
Simpira | 33.210.104 | 28.408.658 | 20.439.600 | 63.489.536 | 62.707.712

The ARM code can be used to cross-compile binaries which run the SPHINCS benchmarks, while the x86 code can be
directly used with [SUPERCOP](https://bench.cr.yp.to/supercop.html).

Note that this code should <b>NOT</b> be used in production and its sole purpose is to evaluate the performance
of instantiating SPHINCS with different cryptographic hash functions.
