#include <stdlib.h>
#include <stdio.h>

#define _GNU_SOURCE             /* See feature_test_macros(7) */
#define __USE_GNU
#include <sched.h>

#include "randombytes.h"
#include "cpucycles.h"
#include "crypto_sign.h"
#include "../timing.h"

extern void printentry(long long,const char *,long long *,long long);
extern unsigned char *alignedcalloc(unsigned long long);
extern const char *primitiveimplementation;
extern const char *implementationversion;
extern const char *sizenames[];
extern const long long sizes[];
extern void allocate(void);
extern void measure(void);

const char *primitiveimplementation = crypto_sign_IMPLEMENTATION;
const char *implementationversion = crypto_sign_VERSION;
const char *sizenames[] = { "outputbytes", "publickeybytes", "secretkeybytes", 0 };
const long long sizes[] = { crypto_sign_BYTES, crypto_sign_PUBLICKEYBYTES, crypto_sign_SECRETKEYBYTES };

//#define MAXTEST_BYTES 100000
#define MAXTEST_BYTES 59
#define LOOPS 10

static unsigned char *pk;
static unsigned char *sk;
static unsigned char *m; unsigned long long mlen;
static unsigned char *sm; unsigned long long smlen;
static unsigned char *t; unsigned long long tlen;

void preallocate(void)
{
#ifdef RAND_R_PRNG_NOT_SEEDED
  RAND_status();
#endif
}

void fail(const char *why)
{
  fprintf(stderr,"measure: fatal: %s\n",why);
  exit(111);
}

unsigned char *alignedcalloc(unsigned long long len)
{
  unsigned char *x = (unsigned char *) calloc(1,len + 128);
  if (!x) fail("out of memory");
  /* will never deallocate so shifting is ok */
  x += 63 & (-(unsigned long) x);
  return x;
}



void printword(const char *s)
{
  if (!*s) putchar('-');
  while (*s) {
    if (*s == ' ') putchar('_');
    else if (*s == '\t') putchar('_');
    else if (*s == '\r') putchar('_');
    else if (*s == '\n') putchar('_');
    else putchar(*s);
    ++s;
  }
  putchar(' ');
}

static void printnum(long long x)
{
  printf("%lld ",x);
}

void printentry(long long mbytes,const char *measuring,long long *m,long long mlen)
{
  long long i;
  long long j;
  long long belowj;
  long long abovej;

  printword(measuring);
  if (mbytes >= 0) printnum(mbytes); else printword("");
  if (mlen > 0) {
    for (j = 0;j + 1 < mlen;++j) {
      belowj = 0;
      for (i = 0;i < mlen;++i) if (m[i] < m[j]) ++belowj;
      abovej = 0;
      for (i = 0;i < mlen;++i) if (m[i] > m[j]) ++abovej;
      if (belowj * 2 < mlen && abovej * 2 < mlen) break;
    }
    printnum(m[j]);
    if (mlen > 1) {
      for (i = 0;i < mlen;++i) printnum(m[i]);
    }
  }
  printf("\n"); fflush(stdout);
}

void allocate(void)
{
  pk = alignedcalloc(crypto_sign_PUBLICKEYBYTES);
  sk = alignedcalloc(crypto_sign_SECRETKEYBYTES);
  m = alignedcalloc(MAXTEST_BYTES + crypto_sign_BYTES);
  sm = alignedcalloc(MAXTEST_BYTES + crypto_sign_BYTES);
  t = alignedcalloc(MAXTEST_BYTES + crypto_sign_BYTES);
}

#define TIMINGS 20
static long long cycles[TIMINGS + 1];
static long long bytes[TIMINGS + 1];

void measure(void)
{
	int i;
	int loop;

    cpu_set_t cpuset;
    CPU_ZERO(&cpuset); CPU_SET(CPU_ID, &cpuset);

    if (sched_setaffinity(getpid(), sizeof cpuset, &cpuset) != 0) perror("setaffinity");

	for (loop = 0;loop < LOOPS;++loop) {
		for (i = 0;i <= TIMINGS;++i) {
			unsigned long long tmp = startTimer();
			crypto_sign_keypair(pk,sk);
            cycles[i] = endTimer() - tmp;
		}
		printentry(-1,"keypair_cycles",cycles,TIMINGS);

		for (mlen = 59;mlen <= MAXTEST_BYTES;mlen += 1 + mlen / 4) {
			randombytes(m,mlen);

			for (i = 0;i <= TIMINGS;++i) {
			    unsigned long long tmp = startTimer();
				bytes[i] = crypto_sign(sm,&smlen,m,mlen,sk);
                cycles[i] = endTimer() - tmp;
				if (bytes[i] == 0)
					bytes[i] = smlen;
			}
			printentry(mlen,"cycles",cycles,TIMINGS);

			for (i = 0;i <= TIMINGS;++i) {
			    unsigned long long tmp = startTimer();
				bytes[i] = crypto_sign_open(t,&tlen,sm,smlen,pk);
                cycles[i] = endTimer() - tmp;
				if (bytes[i] == 0) bytes[i] = tlen;
			}
			printentry(mlen,"open_cycles",cycles,TIMINGS);
		}
	}
}

int main() {
    preallocate();
    allocate();
    measure();
}
