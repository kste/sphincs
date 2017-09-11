/*
Implementation based on the KeccakP-1600 implementation by
the Keccak, Keyak and Ketje Teams, namely, Guido Bertoni, Joan Daemen,
Michaël Peeters, Gilles Van Assche and Ronny Van Keer.

To the extent possible under law, the implementer has waived all copyright
and related or neighboring rights to the source code in this file.
http://creativecommons.org/publicdomain/zero/1.0/
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <smmintrin.h>
#include <stdint.h>
#include <wmmintrin.h>
#include <immintrin.h>
#include <emmintrin.h>
#include "align.h"
#include "KeccakP-800-times8-SnP.h"
#include "SIMD256-config.h"

#include "brg_endian.h"
#if (PLATFORM_BYTE_ORDER != IS_LITTLE_ENDIAN)
#error Expecting a little-endian platform
#endif

typedef unsigned char UINT8;
typedef unsigned long long int UINT64;
typedef uint32_t UINT32;
typedef __m128i V128;
typedef __m256i V256;

#define ANDnu256(a, b)          _mm256_andnot_si256(a, b)
#define CONST256(a)             _mm256_loadu_si256((const V256 *)&(a))
#define CONST256_64(a)          (V256)_mm256_broadcast_sd((const double*)(&a))
#define LOAD256(a)              _mm256_load_si256((const V256 *)&(a))
#define LOAD256u(a)             _mm256_loadu_si256((const V256 *)&(a))
#define LOAD4_64(a, b, c, d)    _mm256_set_epi64x((UINT64)(a), (UINT64)(b), (UINT64)(c), (UINT64)(d))
#define LOAD8_32(a, b, c, d, e, f, g, h) _mm256_set_epi32((UINT32)(a), (UINT32)(b), (UINT32)(c), (UINT32)(d), (UINT32)(e), (UINT32)(f), (UINT32)(g), (UINT32)(h))
#define ROL32in256(d, a, o)     d = _mm256_or_si256(_mm256_slli_epi32(a, (o % 32)), _mm256_srli_epi32(a, 32-(o % 32)))
#define ROL32in256_8(d, a)      d = _mm256_shuffle_epi8(a, CONST256(rho8))
#define ROL32in256_56(d, a)     d = _mm256_shuffle_epi8(a, CONST256(rho56))
//TODO: Add fast rotation for 24 and fix 8 for 32-bit lanes
static const UINT64 rho8[4] = {0x0605040302010007, 0x0E0D0C0B0A09080F, 0x1615141312111017, 0x1E1D1C1B1A19181F};
static const UINT64 rho56[4] = {0x0007060504030201, 0x080F0E0D0C0B0A09, 0x1017161514131211, 0x181F1E1D1C1B1A19};
#define STORE256(a, b)          _mm256_store_si256((V256 *)&(a), b)
#define STORE256u(a, b)         _mm256_storeu_si256((V256 *)&(a), b)
#define STORE2_128(ah, al, v)   _mm256_storeu2_m128d((V128*)&(ah), (V128*)&(al), v)
#define XOR256(a, b)            _mm256_xor_si256(a, b)
#define XOReq256(a, b)          a = _mm256_xor_si256(a, b)
#define UNPACKL( a, b )         _mm256_unpacklo_epi64((a), (b))
#define UNPACKH( a, b )         _mm256_unpackhi_epi64((a), (b))
#define PERM128( a, b, c )      (V256)_mm256_permute2f128_ps((__m256)(a), (__m256)(b), c)
#define SHUFFLE64( a, b, c )    (V256)_mm256_shuffle_pd((__m256d)(a), (__m256d)(b), c)

#define UNINTLEAVE()            lanesL01 = UNPACKL( lanes0, lanes1 ),                   \
                                lanesH01 = UNPACKH( lanes0, lanes1 ),                   \
                                lanesL23 = UNPACKL( lanes2, lanes3 ),                   \
                                lanesH23 = UNPACKH( lanes2, lanes3 ),                   \
                                lanes0 = PERM128( lanesL01, lanesL23, 0x20 ),           \
                                lanes2 = PERM128( lanesL01, lanesL23, 0x31 ),           \
                                lanes1 = PERM128( lanesH01, lanesH23, 0x20 ),           \
                                lanes3 = PERM128( lanesH01, lanesH23, 0x31 )

#define INTLEAVE()              lanesL01 = PERM128( lanes0, lanes2, 0x20 ),             \
                                lanesH01 = PERM128( lanes1, lanes3, 0x20 ),             \
                                lanesL23 = PERM128( lanes0, lanes2, 0x31 ),             \
                                lanesH23 = PERM128( lanes1, lanes3, 0x31 ),             \
                                lanes0 = SHUFFLE64( lanesL01, lanesH01, 0x00 ),         \
                                lanes1 = SHUFFLE64( lanesL01, lanesH01, 0x0F ),         \
                                lanes2 = SHUFFLE64( lanesL23, lanesH23, 0x00 ),         \
                                lanes3 = SHUFFLE64( lanesL23, lanesH23, 0x0F )


#define SnP_laneLengthInBytes 4

void KeccakP800times8_InitializeAll(void *states)
{
	memset(states, 0, KeccakP800times8_statesSizeInBytes);
}

void KeccakP800times8_AddLanesAll(void *states, const unsigned char *data, unsigned int laneCount, unsigned int laneOffset)
{
	V256 *stateAsLanes = (V256 *)states;
    unsigned int i;
    const UINT32 *curData0 = (const UINT32 *)data;
    const UINT32 *curData1 = (const UINT32 *)(data+laneOffset*SnP_laneLengthInBytes);
    const UINT32 *curData2 = (const UINT32 *)(data+laneOffset*2*SnP_laneLengthInBytes);
    const UINT32 *curData3 = (const UINT32 *)(data+laneOffset*3*SnP_laneLengthInBytes);
    const UINT32 *curData4 = (const UINT32 *)(data+laneOffset*4*SnP_laneLengthInBytes);
    const UINT32 *curData5 = (const UINT32 *)(data+laneOffset*5*SnP_laneLengthInBytes);
    const UINT32 *curData6 = (const UINT32 *)(data+laneOffset*6*SnP_laneLengthInBytes);
    const UINT32 *curData7 = (const UINT32 *)(data+laneOffset*7*SnP_laneLengthInBytes);
    V256    lanes0, lanes1, lanes2, lanes3, lanes4, lanes5, lanes6, lanes7, lanesL01, lanesL23, lanesH01, lanesH23;

	#define Xor_In( argIndex )  XOReq256(stateAsLanes[argIndex], LOAD8_32(curData7[argIndex], curData6[argIndex], curData5[argIndex], curData4[argIndex], curData3[argIndex], curData2[argIndex], curData1[argIndex], curData0[argIndex]))

    for(i=0; i<laneCount; i++)
        Xor_In( i );
}

void KeccakP800times8_ExtractLanesAll(const void *states, unsigned char *data, unsigned int laneCount, unsigned int laneOffset)
{
	UINT32 *curData0 = (UINT32 *)data;
    UINT32 *curData1 = (UINT32 *)(data+laneOffset*1*SnP_laneLengthInBytes);
    UINT32 *curData2 = (UINT32 *)(data+laneOffset*2*SnP_laneLengthInBytes);
    UINT32 *curData3 = (UINT32 *)(data+laneOffset*3*SnP_laneLengthInBytes);
    UINT32 *curData4 = (UINT32 *)(data+laneOffset*4*SnP_laneLengthInBytes);
    UINT32 *curData5 = (UINT32 *)(data+laneOffset*5*SnP_laneLengthInBytes);
    UINT32 *curData6 = (UINT32 *)(data+laneOffset*6*SnP_laneLengthInBytes);
    UINT32 *curData7 = (UINT32 *)(data+laneOffset*7*SnP_laneLengthInBytes);

    const V256 *stateAsLanes = (const V256 *)states;
    const UINT32 *stateAsLanes32 = (const UINT32*)states;
    V256    lanes0, lanes1, lanes2, lanes3, lanes4, lanes5, lanes6, lanes7, lanesL01, lanesL23, lanesH01, lanesH23;
    unsigned int i;

    #define Extr( argIndex )    curData0[argIndex] = stateAsLanes32[8*(argIndex)],      \
                                curData1[argIndex] = stateAsLanes32[8*(argIndex)+1],    \
                                curData2[argIndex] = stateAsLanes32[8*(argIndex)+2],    \
                                curData3[argIndex] = stateAsLanes32[8*(argIndex)+3],    \
                                curData4[argIndex] = stateAsLanes32[8*(argIndex)+4],    \
                                curData5[argIndex] = stateAsLanes32[8*(argIndex)+5],    \
                                curData6[argIndex] = stateAsLanes32[8*(argIndex)+6],    \
                                curData7[argIndex] = stateAsLanes32[8*(argIndex)+7]

    for(i=0; i<laneCount; i++)
        Extr( i );

}

#define declareABCDE \
    V256 Aba, Abe, Abi, Abo, Abu; \
    V256 Aga, Age, Agi, Ago, Agu; \
    V256 Aka, Ake, Aki, Ako, Aku; \
    V256 Ama, Ame, Ami, Amo, Amu; \
    V256 Asa, Ase, Asi, Aso, Asu; \
    V256 Bba, Bbe, Bbi, Bbo, Bbu; \
    V256 Bga, Bge, Bgi, Bgo, Bgu; \
    V256 Bka, Bke, Bki, Bko, Bku; \
    V256 Bma, Bme, Bmi, Bmo, Bmu; \
    V256 Bsa, Bse, Bsi, Bso, Bsu; \
    V256 Ca, Ce, Ci, Co, Cu; \
    V256 Ca1, Ce1, Ci1, Co1, Cu1; \
    V256 Da, De, Di, Do, Du; \
    V256 Eba, Ebe, Ebi, Ebo, Ebu; \
    V256 Ega, Ege, Egi, Ego, Egu; \
    V256 Eka, Eke, Eki, Eko, Eku; \
    V256 Ema, Eme, Emi, Emo, Emu; \
    V256 Esa, Ese, Esi, Eso, Esu; \

#define prepareTheta \
    Ca = XOR256(Aba, XOR256(Aga, XOR256(Aka, XOR256(Ama, Asa)))); \
    Ce = XOR256(Abe, XOR256(Age, XOR256(Ake, XOR256(Ame, Ase)))); \
    Ci = XOR256(Abi, XOR256(Agi, XOR256(Aki, XOR256(Ami, Asi)))); \
    Co = XOR256(Abo, XOR256(Ago, XOR256(Ako, XOR256(Amo, Aso)))); \
    Cu = XOR256(Abu, XOR256(Agu, XOR256(Aku, XOR256(Amu, Asu)))); \

/* --- Theta Rho Pi Chi Iota Prepare-theta */
/* --- 64-bit lanes mapped to 64-bit words */
#define thetaRhoPiChiIotaPrepareTheta(i, A, E) \
    ROL32in256(Ce1, Ce, 1); \
    Da = XOR256(Cu, Ce1); \
    ROL32in256(Ci1, Ci, 1); \
    De = XOR256(Ca, Ci1); \
    ROL32in256(Co1, Co, 1); \
    Di = XOR256(Ce, Co1); \
    ROL32in256(Cu1, Cu, 1); \
    Do = XOR256(Ci, Cu1); \
    ROL32in256(Ca1, Ca, 1); \
    Du = XOR256(Co, Ca1); \
\
    XOReq256(A##ba, Da); \
    Bba = A##ba; \
    XOReq256(A##ge, De); \
    ROL32in256(Bbe, A##ge, 44); \
    XOReq256(A##ki, Di); \
    ROL32in256(Bbi, A##ki, 43); \
    E##ba = XOR256(Bba, ANDnu256(Bbe, Bbi)); \
    XOReq256(E##ba, CONST256_64(KeccakF800RoundConstants[i])); \
    Ca = E##ba; \
    XOReq256(A##mo, Do); \
    ROL32in256(Bbo, A##mo, 21); \
    E##be = XOR256(Bbe, ANDnu256(Bbi, Bbo)); \
    Ce = E##be; \
    XOReq256(A##su, Du); \
    ROL32in256(Bbu, A##su, 14); \
    E##bi = XOR256(Bbi, ANDnu256(Bbo, Bbu)); \
    Ci = E##bi; \
    E##bo = XOR256(Bbo, ANDnu256(Bbu, Bba)); \
    Co = E##bo; \
    E##bu = XOR256(Bbu, ANDnu256(Bba, Bbe)); \
    Cu = E##bu; \
\
    XOReq256(A##bo, Do); \
    ROL32in256(Bga, A##bo, 28); \
    XOReq256(A##gu, Du); \
    ROL32in256(Bge, A##gu, 20); \
    XOReq256(A##ka, Da); \
    ROL32in256(Bgi, A##ka, 3); \
    E##ga = XOR256(Bga, ANDnu256(Bge, Bgi)); \
    XOReq256(Ca, E##ga); \
    XOReq256(A##me, De); \
    ROL32in256(Bgo, A##me, 45); \
    E##ge = XOR256(Bge, ANDnu256(Bgi, Bgo)); \
    XOReq256(Ce, E##ge); \
    XOReq256(A##si, Di); \
    ROL32in256(Bgu, A##si, 61); \
    E##gi = XOR256(Bgi, ANDnu256(Bgo, Bgu)); \
    XOReq256(Ci, E##gi); \
    E##go = XOR256(Bgo, ANDnu256(Bgu, Bga)); \
    XOReq256(Co, E##go); \
    E##gu = XOR256(Bgu, ANDnu256(Bga, Bge)); \
    XOReq256(Cu, E##gu); \
\
    XOReq256(A##be, De); \
    ROL32in256(Bka, A##be, 1); \
    XOReq256(A##gi, Di); \
    ROL32in256(Bke, A##gi, 6); \
    XOReq256(A##ko, Do); \
    ROL32in256(Bki, A##ko, 25); \
    E##ka = XOR256(Bka, ANDnu256(Bke, Bki)); \
    XOReq256(Ca, E##ka); \
    XOReq256(A##mu, Du); \
    ROL32in256_8(Bko, A##mu); \
    E##ke = XOR256(Bke, ANDnu256(Bki, Bko)); \
    XOReq256(Ce, E##ke); \
    XOReq256(A##sa, Da); \
    ROL32in256(Bku, A##sa, 18); \
    E##ki = XOR256(Bki, ANDnu256(Bko, Bku)); \
    XOReq256(Ci, E##ki); \
    E##ko = XOR256(Bko, ANDnu256(Bku, Bka)); \
    XOReq256(Co, E##ko); \
    E##ku = XOR256(Bku, ANDnu256(Bka, Bke)); \
    XOReq256(Cu, E##ku); \
\
    XOReq256(A##bu, Du); \
    ROL32in256(Bma, A##bu, 27); \
    XOReq256(A##ga, Da); \
    ROL32in256(Bme, A##ga, 36); \
    XOReq256(A##ke, De); \
    ROL32in256(Bmi, A##ke, 10); \
    E##ma = XOR256(Bma, ANDnu256(Bme, Bmi)); \
    XOReq256(Ca, E##ma); \
    XOReq256(A##mi, Di); \
    ROL32in256(Bmo, A##mi, 15); \
    E##me = XOR256(Bme, ANDnu256(Bmi, Bmo)); \
    XOReq256(Ce, E##me); \
    XOReq256(A##so, Do); \
    ROL32in256_56(Bmu, A##so); \
    E##mi = XOR256(Bmi, ANDnu256(Bmo, Bmu)); \
    XOReq256(Ci, E##mi); \
    E##mo = XOR256(Bmo, ANDnu256(Bmu, Bma)); \
    XOReq256(Co, E##mo); \
    E##mu = XOR256(Bmu, ANDnu256(Bma, Bme)); \
    XOReq256(Cu, E##mu); \
\
    XOReq256(A##bi, Di); \
    ROL32in256(Bsa, A##bi, 62); \
    XOReq256(A##go, Do); \
    ROL32in256(Bse, A##go, 55); \
    XOReq256(A##ku, Du); \
    ROL32in256(Bsi, A##ku, 39); \
    E##sa = XOR256(Bsa, ANDnu256(Bse, Bsi)); \
    XOReq256(Ca, E##sa); \
    XOReq256(A##ma, Da); \
    ROL32in256(Bso, A##ma, 41); \
    E##se = XOR256(Bse, ANDnu256(Bsi, Bso)); \
    XOReq256(Ce, E##se); \
    XOReq256(A##se, De); \
    ROL32in256(Bsu, A##se, 2); \
    E##si = XOR256(Bsi, ANDnu256(Bso, Bsu)); \
    XOReq256(Ci, E##si); \
    E##so = XOR256(Bso, ANDnu256(Bsu, Bsa)); \
    XOReq256(Co, E##so); \
    E##su = XOR256(Bsu, ANDnu256(Bsa, Bse)); \
    XOReq256(Cu, E##su); \
\

/* --- Theta Rho Pi Chi Iota */
/* --- 64-bit lanes mapped to 64-bit words */
#define thetaRhoPiChiIota(i, A, E) \
    ROL32in256(Ce1, Ce, 1); \
    Da = XOR256(Cu, Ce1); \
    ROL32in256(Ci1, Ci, 1); \
    De = XOR256(Ca, Ci1); \
    ROL32in256(Co1, Co, 1); \
    Di = XOR256(Ce, Co1); \
    ROL32in256(Cu1, Cu, 1); \
    Do = XOR256(Ci, Cu1); \
    ROL32in256(Ca1, Ca, 1); \
    Du = XOR256(Co, Ca1); \
\
    XOReq256(A##ba, Da); \
    Bba = A##ba; \
    XOReq256(A##ge, De); \
    ROL32in256(Bbe, A##ge, 44); \
    XOReq256(A##ki, Di); \
    ROL32in256(Bbi, A##ki, 43); \
    E##ba = XOR256(Bba, ANDnu256(Bbe, Bbi)); \
    XOReq256(E##ba, CONST256_64(KeccakF800RoundConstants[i])); \
    XOReq256(A##mo, Do); \
    ROL32in256(Bbo, A##mo, 21); \
    E##be = XOR256(Bbe, ANDnu256(Bbi, Bbo)); \
    XOReq256(A##su, Du); \
    ROL32in256(Bbu, A##su, 14); \
    E##bi = XOR256(Bbi, ANDnu256(Bbo, Bbu)); \
    E##bo = XOR256(Bbo, ANDnu256(Bbu, Bba)); \
    E##bu = XOR256(Bbu, ANDnu256(Bba, Bbe)); \
\
    XOReq256(A##bo, Do); \
    ROL32in256(Bga, A##bo, 28); \
    XOReq256(A##gu, Du); \
    ROL32in256(Bge, A##gu, 20); \
    XOReq256(A##ka, Da); \
    ROL32in256(Bgi, A##ka, 3); \
    E##ga = XOR256(Bga, ANDnu256(Bge, Bgi)); \
    XOReq256(A##me, De); \
    ROL32in256(Bgo, A##me, 45); \
    E##ge = XOR256(Bge, ANDnu256(Bgi, Bgo)); \
    XOReq256(A##si, Di); \
    ROL32in256(Bgu, A##si, 61); \
    E##gi = XOR256(Bgi, ANDnu256(Bgo, Bgu)); \
    E##go = XOR256(Bgo, ANDnu256(Bgu, Bga)); \
    E##gu = XOR256(Bgu, ANDnu256(Bga, Bge)); \
\
    XOReq256(A##be, De); \
    ROL32in256(Bka, A##be, 1); \
    XOReq256(A##gi, Di); \
    ROL32in256(Bke, A##gi, 6); \
    XOReq256(A##ko, Do); \
    ROL32in256(Bki, A##ko, 25); \
    E##ka = XOR256(Bka, ANDnu256(Bke, Bki)); \
    XOReq256(A##mu, Du); \
    ROL32in256_8(Bko, A##mu); \
    E##ke = XOR256(Bke, ANDnu256(Bki, Bko)); \
    XOReq256(A##sa, Da); \
    ROL32in256(Bku, A##sa, 18); \
    E##ki = XOR256(Bki, ANDnu256(Bko, Bku)); \
    E##ko = XOR256(Bko, ANDnu256(Bku, Bka)); \
    E##ku = XOR256(Bku, ANDnu256(Bka, Bke)); \
\
    XOReq256(A##bu, Du); \
    ROL32in256(Bma, A##bu, 27); \
    XOReq256(A##ga, Da); \
    ROL32in256(Bme, A##ga, 36); \
    XOReq256(A##ke, De); \
    ROL32in256(Bmi, A##ke, 10); \
    E##ma = XOR256(Bma, ANDnu256(Bme, Bmi)); \
    XOReq256(A##mi, Di); \
    ROL32in256(Bmo, A##mi, 15); \
    E##me = XOR256(Bme, ANDnu256(Bmi, Bmo)); \
    XOReq256(A##so, Do); \
    ROL32in256_56(Bmu, A##so); \
    E##mi = XOR256(Bmi, ANDnu256(Bmo, Bmu)); \
    E##mo = XOR256(Bmo, ANDnu256(Bmu, Bma)); \
    E##mu = XOR256(Bmu, ANDnu256(Bma, Bme)); \
\
    XOReq256(A##bi, Di); \
    ROL32in256(Bsa, A##bi, 62); \
    XOReq256(A##go, Do); \
    ROL32in256(Bse, A##go, 55); \
    XOReq256(A##ku, Du); \
    ROL32in256(Bsi, A##ku, 39); \
    E##sa = XOR256(Bsa, ANDnu256(Bse, Bsi)); \
    XOReq256(A##ma, Da); \
    ROL32in256(Bso, A##ma, 41); \
    E##se = XOR256(Bse, ANDnu256(Bsi, Bso)); \
    XOReq256(A##se, De); \
    ROL32in256(Bsu, A##se, 2); \
    E##si = XOR256(Bsi, ANDnu256(Bso, Bsu)); \
    E##so = XOR256(Bso, ANDnu256(Bsu, Bsa)); \
    E##su = XOR256(Bsu, ANDnu256(Bsa, Bse)); \
\

static ALIGN(KeccakP800times8_statesAlignment) const UINT32 KeccakF800RoundConstants[24] = {
    0x00000001,
    0x00008082,
    0x0000808a,
    0x80008000,
    0x0000808b,
    0x80000001,
    0x80008081,
    0x00008009,
    0x0000008a,
    0x00000088,
    0x80008009,
    0x8000000a,
    0x8000808b,
    0x0000008b,
    0x00008089,
    0x00008003,
    0x00008002,
    0x00000080,
    0x0000800a,
    0x8000000a,
    0x80008081,
    0x00008080,
    0x80000001,
    0x80008008};

#define copyFromState(X, state) \
    X##ba = LOAD256(state[ 0]); \
    X##be = LOAD256(state[ 1]); \
    X##bi = LOAD256(state[ 2]); \
    X##bo = LOAD256(state[ 3]); \
    X##bu = LOAD256(state[ 4]); \
    X##ga = LOAD256(state[ 5]); \
    X##ge = LOAD256(state[ 6]); \
    X##gi = LOAD256(state[ 7]); \
    X##go = LOAD256(state[ 8]); \
    X##gu = LOAD256(state[ 9]); \
    X##ka = LOAD256(state[10]); \
    X##ke = LOAD256(state[11]); \
    X##ki = LOAD256(state[12]); \
    X##ko = LOAD256(state[13]); \
    X##ku = LOAD256(state[14]); \
    X##ma = LOAD256(state[15]); \
    X##me = LOAD256(state[16]); \
    X##mi = LOAD256(state[17]); \
    X##mo = LOAD256(state[18]); \
    X##mu = LOAD256(state[19]); \
    X##sa = LOAD256(state[20]); \
    X##se = LOAD256(state[21]); \
    X##si = LOAD256(state[22]); \
    X##so = LOAD256(state[23]); \
    X##su = LOAD256(state[24]); \

#define copyToState(state, X) \
    STORE256(state[ 0], X##ba); \
    STORE256(state[ 1], X##be); \
    STORE256(state[ 2], X##bi); \
    STORE256(state[ 3], X##bo); \
    STORE256(state[ 4], X##bu); \
    STORE256(state[ 5], X##ga); \
    STORE256(state[ 6], X##ge); \
    STORE256(state[ 7], X##gi); \
    STORE256(state[ 8], X##go); \
    STORE256(state[ 9], X##gu); \
    STORE256(state[10], X##ka); \
    STORE256(state[11], X##ke); \
    STORE256(state[12], X##ki); \
    STORE256(state[13], X##ko); \
    STORE256(state[14], X##ku); \
    STORE256(state[15], X##ma); \
    STORE256(state[16], X##me); \
    STORE256(state[17], X##mi); \
    STORE256(state[18], X##mo); \
    STORE256(state[19], X##mu); \
    STORE256(state[20], X##sa); \
    STORE256(state[21], X##se); \
    STORE256(state[22], X##si); \
    STORE256(state[23], X##so); \
    STORE256(state[24], X##su); \

#define copyStateVariables(X, Y) \
    X##ba = Y##ba; \
    X##be = Y##be; \
    X##bi = Y##bi; \
    X##bo = Y##bo; \
    X##bu = Y##bu; \
    X##ga = Y##ga; \
    X##ge = Y##ge; \
    X##gi = Y##gi; \
    X##go = Y##go; \
    X##gu = Y##gu; \
    X##ka = Y##ka; \
    X##ke = Y##ke; \
    X##ki = Y##ki; \
    X##ko = Y##ko; \
    X##ku = Y##ku; \
    X##ma = Y##ma; \
    X##me = Y##me; \
    X##mi = Y##mi; \
    X##mo = Y##mo; \
    X##mu = Y##mu; \
    X##sa = Y##sa; \
    X##se = Y##se; \
    X##si = Y##si; \
    X##so = Y##so; \
    X##su = Y##su; \


#define FullUnrolling
#include "KeccakP-1600-unrolling.macros"

void KeccakP800times8_PermuteAll_12rounds(void *states)
{
    V256 *statesAsLanes = (V256 *)states;
    declareABCDE
    #ifndef KeccakP800times4_fullUnrolling
    unsigned int i;
    #endif

    copyFromState(A, statesAsLanes)
    rounds12
    copyToState(statesAsLanes, A)
}
