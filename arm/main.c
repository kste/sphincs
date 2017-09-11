/*
This code measures the performance of optimized implementations of short-input
hash functions, which either take 256-bit (F) or 512-bit (H) as input and
produce a 256-bit output. One of the main applications for such functions are
hash-based signature schemes like SPHINCS (https://sphincs.cr.yp.to/), where
the bottleneck is the larger number of calls to these short input hash
functions.

For a more detailed description of the choice of functions and their properties
see (link to eprint).
*/

#include "stdio.h"
#include "stdlib.h"
#include <string.h>

#define _GNU_SOURCE
#define __USE_GNU
#include <sched.h>
#include <sys/types.h>
#include <unistd.h>

#include "./haraka/haraka.h"
#include "./chacha/chachaperm.h"
#include "./simpira/simpira.h"
#include "./keccak/keccak.h"
#include "./sha256/sha256.h"
#include "timing.h"

typedef void (*hash_function)(unsigned char*, const unsigned char*);

// Measures how many cycles func requires to process a random input.
double timeit(hash_function func, int inlen, int outlen) {
  unsigned char *in, *out;
  unsigned long long timer = 0;
  double timings[NUM_TIMINGS];

  int i, j;

  srand(0);
  in = malloc(inlen);
  out = malloc(outlen);

  for (i = -100; i < NUM_TIMINGS; i++) {
    // Get random input
    for (j = 0; j < inlen; j++) {
      in[j] = rand() & 0xff;
    }

    timer = startTimer();
    for(j = 0; j < ITERATIONS; j++) {
        func(out, in);
    }
    timer = endTimer() - timer;

    if (i >= 0 && i < NUM_TIMINGS) {
      timings[i] = ((double)timer) / inlen / ITERATIONS;
    }
  }

  // Sort to extract median value.
  qsort(timings, NUM_TIMINGS, sizeof(double), compareDouble);

  free(out);
  free(in);
  return timings[NUM_TIMINGS / 2];
}

int main() {
  // Fix the CPU on which the benchmark should run.
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset); CPU_SET(CPU_ID, &cpuset);

  if (sched_setaffinity(getpid(), sizeof cpuset, &cpuset) != 0) {
    perror("setaffinity");
  }

  // Test correctness of implementations.
  haraka_testvectors();
  simpira_testvectors();
  sha256_testvectors();

  printf("Haraka-F 1x: %f cycles per byte\n", timeit(haraka_f, 32, 32));
  printf("Haraka-F 4x: %f cycles per byte\n", timeit(haraka_f_4x, 4*32, 4*32));
  printf("Haraka-F 8x: %f cycles per byte\n", timeit(haraka_f_8x, 8*32, 8*32));
  printf("Haraka-H 1x: %f cycles per byte\n", timeit(haraka_h, 64, 32));
  printf("Haraka-H 4x: %f cycles per byte\n", timeit(haraka_h_4x, 4*64, 4*32));
  printf("Haraka-H 8x: %f cycles per byte\n", timeit(haraka_h_8x, 8*64, 8*32));

  printf("Simpira-F 1x: %f cycles per byte\n", timeit(simpira_f, 32, 32));
  printf("Simpira-F 4x: %f cycles per byte\n", timeit(simpira_f_4x, 4*32, 4*32));
  printf("Simpira-F 8x: %f cycles per byte\n", timeit(simpira_f_8x, 8*32, 8*32));
  printf("Simpira-H 1x: %f cycles per byte\n", timeit(simpira_h, 64, 64));
  printf("Simpira-H 4x: %f cycles per byte\n", timeit(simpira_h_4x, 4*64, 4*64));
  printf("Simpira-H 8x: %f cycles per byte\n", timeit(simpira_h_8x, 8*64, 8*64));

  printf("ChaCha12-F: %f cycles per byte\n", timeit(chacha_f, 32, 64));
  printf("ChaCha12-F 8x: %f cycles per byte\n", timeit(chacha_f_8x, 8*32, 8*64));
  printf("ChaCha12-H: %f cycles per byte\n", timeit(chacha_h, 64, 64));
  printf("ChaCha12-H 8x: %f cycles per byte\n", timeit(chacha_h_8x, 8*64, 8*64));

  printf("Keccak[r=12, b=800]-F 1x: %f cycles per byte\n", timeit(KeccakP800_f, 32, 32));
  printf("Keccak[r=12, b=800]-F 4x: %f cycles per byte\n", timeit(KeccakP800_f_4x, 4*32, 4*32));
  printf("Keccak[r=12, b=800]-H 1x: %f cycles per byte\n", timeit(KeccakP800_h, 64, 32));
  printf("Keccak[r=12, b=800]-H 4x: %f cycles per byte\n", timeit(KeccakP800_h_4x, 4*64, 4*32));

  printf("SHA256-F: %f cycles per byte\n", timeit(sha256_f, 32, 32));
  printf("SHA256-F 8x: %f cycles per byte\n", timeit(sha256_f_8x, 8*32, 8*32));
  printf("SHA256-H: %f cycles per byte\n", timeit(sha256_h, 64, 32));
  printf("SHA256-H 8x: %f cycles per byte\n", timeit(sha256_h_8x, 8*64, 8*32));
}
