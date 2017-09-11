# Cross compile for ARM
CC=aarch64-linux-gnu-gcc
AS=aarch64-linux-gnu-as
CFLAGS=-O3 -mcpu=cortex-a72+crypto -fomit-frame-pointer -s -static

# Native compile on ARM
# CC=gcc
# CFLAGS=-O3 -march=native -fomit-frame-pointer
RM=rm -f
SRCS=haraka/haraka.c chacha/chachaperm.c simpira/simpira.c sha256/sha256.c \
     keccak/keccak.c keccak/keccak800.c keccak/KeccakP-800-armv8a-neon.s \
	 sphincs/hash.c sphincs/horst.c sphincs/prg.c sphincs/randombytes.c \
	 sphincs/sign.c sphincs/wots.c sphincs/zerobytes.c sphincs/blake/hash.c \
	 sphincs/blake/blake.c sphincs/chacha12/chacha.c sphincs/chacha12/stream.c

OBJS=$(subst .c,.o,$(subst .s,.o, $(SRCS)))

.PHONY: all clean sphincs/hash.c

all: arm_benchmark arm_sphincs_chacha arm_sphincs_haraka arm_sphincs_keccak \
	arm_sphincs_simpira arm_sphincs_sha256
	for t in $^; do \
		$(MAKE) $$t;\
	done

arm_benchmark: main.o $(OBJS)
	$(CC) -static -o "arm_shorthash" main.o $(OBJS) $(LDLIBS)

arm_sphincs_chacha: CFLAGS += -DCHACHA
arm_sphincs_chacha: sphincs/measure.o $(OBJS)
	$(CC) -static -o "arm_sphincs_chacha" sphincs/measure.o $(OBJS) $(LDLIBS)

arm_sphincs_haraka: CFLAGS += -DHARAKA
arm_sphincs_haraka: sphincs/measure.o $(OBJS)
	$(CC) -static -o "arm_sphincs_haraka" sphincs/measure.o $(OBJS) $(LDLIBS)

arm_sphincs_keccak: CFLAGS += -DKECCAK
arm_sphincs_keccak: sphincs/measure.o $(OBJS)
	$(CC) -static -o "arm_sphincs_keccak" sphincs/measure.o $(OBJS) $(LDLIBS)

arm_sphincs_simpira: CFLAGS += -DSIMPIRA
arm_sphincs_simpira: sphincs/measure.o $(OBJS)
	$(CC) -static -o "arm_sphincs_simpira" sphincs/measure.o $(OBJS) $(LDLIBS)

arm_sphincs_sha256: CFLAGS += -DSHA256
arm_sphincs_sha256: sphincs/measure.o $(OBJS)
	$(CC) -static -o "arm_sphincs_sha256" sphincs/measure.o $(OBJS) $(LDLIBS)

clean:
	$(RM) $(OBJS) main.o sphincs/measure.o
