#ifndef KECCAK800_H_
#define KECCAK800_H_

#include <arm_neon.h>

typedef unsigned int u32;


#define KeccakP800times4_statesSizeInBytes 400
#define SnP_laneLengthInBytes 4

#define XOR128(a, b) veorq_u32(a, b)
#define XOReq128(a, b) a = veorq_u32(a, b)
#define ANDnu128(a, b) vbicq_u32(b, a)
#define LOAD128(src) vld1q_u32(src)
#define STORE128(dest,src) vst1q_u32(dest,src)
#define ROL32in128(d,a,b) d = vsliq_n_u32(vshrq_n_u32(a, (32-b) % 32),a,b % 32)

void KeccakP800times4_InitializeAll(void *states);
void KeccakP800times4_AddLanesAll(void *states, const unsigned char *data, unsigned int laneCount, unsigned int laneOffset);
void KeccakP800times4_PermuteAll_12rounds(void *states);
void KeccakP800times4_PermuteAll_24rounds(void *states);
void KeccakP800times4_ExtractLanesAll(const void *states, unsigned char *data, unsigned int laneCount, unsigned int laneOffset);

#endif
