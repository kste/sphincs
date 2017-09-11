#include "keccak800.h"
#include <string.h>

void KeccakP800times4_InitializeAll(void *states) {
  memset(states, 0, KeccakP800times4_statesSizeInBytes);
}

void KeccakP800times4_AddLanesAll(void *states, const unsigned char *data, unsigned int laneCount, unsigned int laneOffset) {
  uint32x4_t *stateAsLanes = (uint32x4_t *)states;
  unsigned int i;

  for(i=0; i<laneCount; i++) {
    uint32x4_t buf = {*(data + i),
                      *((data + i + laneOffset*SnP_laneLengthInBytes)),
                      *((data + i + 2*laneOffset*SnP_laneLengthInBytes)),
                      *((data + i + 3*laneOffset*SnP_laneLengthInBytes))};
    XOReq128(stateAsLanes[i], buf);
  }
}


#define declareABCDE \
    uint32x4_t Aba, Abe, Abi, Abo, Abu; \
    uint32x4_t Aga, Age, Agi, Ago, Agu; \
    uint32x4_t Aka, Ake, Aki, Ako, Aku; \
    uint32x4_t Ama, Ame, Ami, Amo, Amu; \
    uint32x4_t Asa, Ase, Asi, Aso, Asu; \
    uint32x4_t Bba, Bbe, Bbi, Bbo, Bbu; \
    uint32x4_t Bga, Bge, Bgi, Bgo, Bgu; \
    uint32x4_t Bka, Bke, Bki, Bko, Bku; \
    uint32x4_t Bma, Bme, Bmi, Bmo, Bmu; \
    uint32x4_t Bsa, Bse, Bsi, Bso, Bsu; \
    uint32x4_t Ca, Ce, Ci, Co, Cu; \
    uint32x4_t Ca1, Ce1, Ci1, Co1, Cu1; \
    uint32x4_t Da, De, Di, Do, Du; \
    uint32x4_t Eba, Ebe, Ebi, Ebo, Ebu; \
    uint32x4_t Ega, Ege, Egi, Ego, Egu; \
    uint32x4_t Eka, Eke, Eki, Eko, Eku; \
    uint32x4_t Ema, Eme, Emi, Emo, Emu; \
    uint32x4_t Esa, Ese, Esi, Eso, Esu; \

#define prepareTheta \
    Ca = XOR128(Aba, XOR128(Aga, XOR128(Aka, XOR128(Ama, Asa)))); \
    Ce = XOR128(Abe, XOR128(Age, XOR128(Ake, XOR128(Ame, Ase)))); \
    Ci = XOR128(Abi, XOR128(Agi, XOR128(Aki, XOR128(Ami, Asi)))); \
    Co = XOR128(Abo, XOR128(Ago, XOR128(Ako, XOR128(Amo, Aso)))); \
    Cu = XOR128(Abu, XOR128(Agu, XOR128(Aku, XOR128(Amu, Asu)))); \

    #define thetaRhoPiChiIotaPrepareTheta(i, A, E) \
        ROL32in128(Ce1, Ce, 1); \
        Da = XOR128(Cu, Ce1); \
        ROL32in128(Ci1, Ci, 1); \
        De = XOR128(Ca, Ci1); \
        ROL32in128(Co1, Co, 1); \
        Di = XOR128(Ce, Co1); \
        ROL32in128(Cu1, Cu, 1); \
        Do = XOR128(Ci, Cu1); \
        ROL32in128(Ca1, Ca, 1); \
        Du = XOR128(Co, Ca1); \
    \
        XOReq128(A##ba, Da); \
        Bba = A##ba; \
        XOReq128(A##ge, De); \
        ROL32in128(Bbe, A##ge, 44); \
        XOReq128(A##ki, Di); \
        ROL32in128(Bbi, A##ki, 43); \
        E##ba = XOR128(Bba, ANDnu128(Bbe, Bbi)); \
        XOReq128(E##ba, KeccakF800RoundConstants[i]); \
        Ca = E##ba; \
        XOReq128(A##mo, Do); \
        ROL32in128(Bbo, A##mo, 21); \
        E##be = XOR128(Bbe, ANDnu128(Bbi, Bbo)); \
        Ce = E##be; \
        XOReq128(A##su, Du); \
        ROL32in128(Bbu, A##su, 14); \
        E##bi = XOR128(Bbi, ANDnu128(Bbo, Bbu)); \
        Ci = E##bi; \
        E##bo = XOR128(Bbo, ANDnu128(Bbu, Bba)); \
        Co = E##bo; \
        E##bu = XOR128(Bbu, ANDnu128(Bba, Bbe)); \
        Cu = E##bu; \
    \
        XOReq128(A##bo, Do); \
        ROL32in128(Bga, A##bo, 28); \
        XOReq128(A##gu, Du); \
        ROL32in128(Bge, A##gu, 20); \
        XOReq128(A##ka, Da); \
        ROL32in128(Bgi, A##ka, 3); \
        E##ga = XOR128(Bga, ANDnu128(Bge, Bgi)); \
        XOReq128(Ca, E##ga); \
        XOReq128(A##me, De); \
        ROL32in128(Bgo, A##me, 45); \
        E##ge = XOR128(Bge, ANDnu128(Bgi, Bgo)); \
        XOReq128(Ce, E##ge); \
        XOReq128(A##si, Di); \
        ROL32in128(Bgu, A##si, 61); \
        E##gi = XOR128(Bgi, ANDnu128(Bgo, Bgu)); \
        XOReq128(Ci, E##gi); \
        E##go = XOR128(Bgo, ANDnu128(Bgu, Bga)); \
        XOReq128(Co, E##go); \
        E##gu = XOR128(Bgu, ANDnu128(Bga, Bge)); \
        XOReq128(Cu, E##gu); \
    \
        XOReq128(A##be, De); \
        ROL32in128(Bka, A##be, 1); \
        XOReq128(A##gi, Di); \
        ROL32in128(Bke, A##gi, 6); \
        XOReq128(A##ko, Do); \
        ROL32in128(Bki, A##ko, 25); \
        E##ka = XOR128(Bka, ANDnu128(Bke, Bki)); \
        XOReq128(Ca, E##ka); \
        XOReq128(A##mu, Du); \
        ROL32in128(Bko, A##mu, 8); \
        E##ke = XOR128(Bke, ANDnu128(Bki, Bko)); \
        XOReq128(Ce, E##ke); \
        XOReq128(A##sa, Da); \
        ROL32in128(Bku, A##sa, 18); \
        E##ki = XOR128(Bki, ANDnu128(Bko, Bku)); \
        XOReq128(Ci, E##ki); \
        E##ko = XOR128(Bko, ANDnu128(Bku, Bka)); \
        XOReq128(Co, E##ko); \
        E##ku = XOR128(Bku, ANDnu128(Bka, Bke)); \
        XOReq128(Cu, E##ku); \
    \
        XOReq128(A##bu, Du); \
        ROL32in128(Bma, A##bu, 27); \
        XOReq128(A##ga, Da); \
        ROL32in128(Bme, A##ga, 36); \
        XOReq128(A##ke, De); \
        ROL32in128(Bmi, A##ke, 10); \
        E##ma = XOR128(Bma, ANDnu128(Bme, Bmi)); \
        XOReq128(Ca, E##ma); \
        XOReq128(A##mi, Di); \
        ROL32in128(Bmo, A##mi, 15); \
        E##me = XOR128(Bme, ANDnu128(Bmi, Bmo)); \
        XOReq128(Ce, E##me); \
        XOReq128(A##so, Do); \
        ROL32in128(Bmu, A##so, 56); \
        E##mi = XOR128(Bmi, ANDnu128(Bmo, Bmu)); \
        XOReq128(Ci, E##mi); \
        E##mo = XOR128(Bmo, ANDnu128(Bmu, Bma)); \
        XOReq128(Co, E##mo); \
        E##mu = XOR128(Bmu, ANDnu128(Bma, Bme)); \
        XOReq128(Cu, E##mu); \
    \
        XOReq128(A##bi, Di); \
        ROL32in128(Bsa, A##bi, 62); \
        XOReq128(A##go, Do); \
        ROL32in128(Bse, A##go, 55); \
        XOReq128(A##ku, Du); \
        ROL32in128(Bsi, A##ku, 39); \
        E##sa = XOR128(Bsa, ANDnu128(Bse, Bsi)); \
        XOReq128(Ca, E##sa); \
        XOReq128(A##ma, Da); \
        ROL32in128(Bso, A##ma, 41); \
        E##se = XOR128(Bse, ANDnu128(Bsi, Bso)); \
        XOReq128(Ce, E##se); \
        XOReq128(A##se, De); \
        ROL32in128(Bsu, A##se, 2); \
        E##si = XOR128(Bsi, ANDnu128(Bso, Bsu)); \
        XOReq128(Ci, E##si); \
        E##so = XOR128(Bso, ANDnu128(Bsu, Bsa)); \
        XOReq128(Co, E##so); \
        E##su = XOR128(Bsu, ANDnu128(Bsa, Bse)); \
        XOReq128(Cu, E##su); \
    \

    /* --- Theta Rho Pi Chi Iota */
    /* --- 64-bit lanes mapped to 64-bit words */
    #define thetaRhoPiChiIota(i, A, E) \
        ROL32in128(Ce1, Ce, 1); \
        Da = XOR128(Cu, Ce1); \
        ROL32in128(Ci1, Ci, 1); \
        De = XOR128(Ca, Ci1); \
        ROL32in128(Co1, Co, 1); \
        Di = XOR128(Ce, Co1); \
        ROL32in128(Cu1, Cu, 1); \
        Do = XOR128(Ci, Cu1); \
        ROL32in128(Ca1, Ca, 1); \
        Du = XOR128(Co, Ca1); \
    \
        XOReq128(A##ba, Da); \
        Bba = A##ba; \
        XOReq128(A##ge, De); \
        ROL32in128(Bbe, A##ge, 44); \
        XOReq128(A##ki, Di); \
        ROL32in128(Bbi, A##ki, 43); \
        E##ba = XOR128(Bba, ANDnu128(Bbe, Bbi)); \
        XOReq128(E##ba, KeccakF800RoundConstants[i]); \
        XOReq128(A##mo, Do); \
        ROL32in128(Bbo, A##mo, 21); \
        E##be = XOR128(Bbe, ANDnu128(Bbi, Bbo)); \
        XOReq128(A##su, Du); \
        ROL32in128(Bbu, A##su, 14); \
        E##bi = XOR128(Bbi, ANDnu128(Bbo, Bbu)); \
        E##bo = XOR128(Bbo, ANDnu128(Bbu, Bba)); \
        E##bu = XOR128(Bbu, ANDnu128(Bba, Bbe)); \
    \
        XOReq128(A##bo, Do); \
        ROL32in128(Bga, A##bo, 28); \
        XOReq128(A##gu, Du); \
        ROL32in128(Bge, A##gu, 20); \
        XOReq128(A##ka, Da); \
        ROL32in128(Bgi, A##ka, 3); \
        E##ga = XOR128(Bga, ANDnu128(Bge, Bgi)); \
        XOReq128(A##me, De); \
        ROL32in128(Bgo, A##me, 45); \
        E##ge = XOR128(Bge, ANDnu128(Bgi, Bgo)); \
        XOReq128(A##si, Di); \
        ROL32in128(Bgu, A##si, 61); \
        E##gi = XOR128(Bgi, ANDnu128(Bgo, Bgu)); \
        E##go = XOR128(Bgo, ANDnu128(Bgu, Bga)); \
        E##gu = XOR128(Bgu, ANDnu128(Bga, Bge)); \
    \
        XOReq128(A##be, De); \
        ROL32in128(Bka, A##be, 1); \
        XOReq128(A##gi, Di); \
        ROL32in128(Bke, A##gi, 6); \
        XOReq128(A##ko, Do); \
        ROL32in128(Bki, A##ko, 25); \
        E##ka = XOR128(Bka, ANDnu128(Bke, Bki)); \
        XOReq128(A##mu, Du); \
        ROL32in128(Bko, A##mu, 8); \
        E##ke = XOR128(Bke, ANDnu128(Bki, Bko)); \
        XOReq128(A##sa, Da); \
        ROL32in128(Bku, A##sa, 18); \
        E##ki = XOR128(Bki, ANDnu128(Bko, Bku)); \
        E##ko = XOR128(Bko, ANDnu128(Bku, Bka)); \
        E##ku = XOR128(Bku, ANDnu128(Bka, Bke)); \
    \
        XOReq128(A##bu, Du); \
        ROL32in128(Bma, A##bu, 27); \
        XOReq128(A##ga, Da); \
        ROL32in128(Bme, A##ga, 36); \
        XOReq128(A##ke, De); \
        ROL32in128(Bmi, A##ke, 10); \
        E##ma = XOR128(Bma, ANDnu128(Bme, Bmi)); \
        XOReq128(A##mi, Di); \
        ROL32in128(Bmo, A##mi, 15); \
        E##me = XOR128(Bme, ANDnu128(Bmi, Bmo)); \
        XOReq128(A##so, Do); \
        ROL32in128(Bmu, A##so, 56); \
        E##mi = XOR128(Bmi, ANDnu128(Bmo, Bmu)); \
        E##mo = XOR128(Bmo, ANDnu128(Bmu, Bma)); \
        E##mu = XOR128(Bmu, ANDnu128(Bma, Bme)); \
    \
        XOReq128(A##bi, Di); \
        ROL32in128(Bsa, A##bi, 62); \
        XOReq128(A##go, Do); \
        ROL32in128(Bse, A##go, 55); \
        XOReq128(A##ku, Du); \
        ROL32in128(Bsi, A##ku, 39); \
        E##sa = XOR128(Bsa, ANDnu128(Bse, Bsi)); \
        XOReq128(A##ma, Da); \
        ROL32in128(Bso, A##ma, 41); \
        E##se = XOR128(Bse, ANDnu128(Bsi, Bso)); \
        XOReq128(A##se, De); \
        ROL32in128(Bsu, A##se, 2); \
        E##si = XOR128(Bsi, ANDnu128(Bso, Bsu)); \
        E##so = XOR128(Bso, ANDnu128(Bsu, Bsa)); \
        E##su = XOR128(Bsu, ANDnu128(Bsa, Bse)); \
    \

    static const uint32x4_t KeccakF800RoundConstants[24] = {
        {0x00000001,0x00000001,0x00000001,0x00000001},
        {0x00008082,0x00008082,0x00008082,0x00008082},
        {0x0000808a,0x0000808a,0x0000808a,0x0000808a},
        {0x80008000,0x80008000,0x80008000,0x80008000},
        {0x0000808b,0x0000808b,0x0000808b,0x0000808b},
        {0x80000001,0x80000001,0x80000001,0x80000001},
        {0x80008081,0x80008081,0x80008081,0x80008081},
        {0x00008009,0x00008009,0x00008009,0x00008009},
        {0x0000008a,0x0000008a,0x0000008a,0x0000008a},
        {0x00000088,0x00000088,0x00000088,0x00000088},
        {0x80008009,0x80008009,0x80008009,0x80008009},
        {0x8000000a,0x8000000a,0x8000000a,0x8000000a},
        {0x8000808b,0x8000808b,0x8000808b,0x8000808b},
        {0x0000008b,0x0000008b,0x0000008b,0x0000008b},
        {0x00008089,0x00008089,0x00008089,0x00008089},
        {0x00008003,0x00008003,0x00008003,0x00008003},
        {0x00008002,0x00008002,0x00008002,0x00008002},
        {0x00000080,0x00000080,0x00000080,0x00000080},
        {0x0000800a,0x0000800a,0x0000800a,0x0000800a},
        {0x8000000a,0x8000000a,0x8000000a,0x8000000a},
        {0x80008081,0x80008081,0x80008081,0x80008081},
        {0x00008080,0x00008080,0x00008080,0x00008080},
        {0x80000001,0x80000001,0x80000001,0x80000001},
        {0x80008008,0x80008008,0x80008008,0x80008008}};

    #define copyFromState(X, state) \
        X##ba = LOAD128(state[ 0]); \
        X##be = LOAD128(state[ 2]); \
        X##bi = LOAD128(state[ 4]); \
        X##bo = LOAD128(state[ 6]); \
        X##bu = LOAD128(state[ 8]); \
        X##ga = LOAD128(state[10]); \
        X##ge = LOAD128(state[12]); \
        X##gi = LOAD128(state[14]); \
        X##go = LOAD128(state[16]); \
        X##gu = LOAD128(state[18]); \
        X##ka = LOAD128(state[20]); \
        X##ke = LOAD128(state[22]); \
        X##ki = LOAD128(state[24]); \
        X##ko = LOAD128(state[26]); \
        X##ku = LOAD128(state[28]); \
        X##ma = LOAD128(state[30]); \
        X##me = LOAD128(state[32]); \
        X##mi = LOAD128(state[34]); \
        X##mo = LOAD128(state[36]); \
        X##mu = LOAD128(state[38]); \
        X##sa = LOAD128(state[40]); \
        X##se = LOAD128(state[42]); \
        X##si = LOAD128(state[44]); \
        X##so = LOAD128(state[46]); \
        X##su = LOAD128(state[48]); \

    #define copyToState(state, X) \
        STORE128(state[ 0], X##ba); \
        STORE128(state[ 2], X##be); \
        STORE128(state[ 4], X##bi); \
        STORE128(state[ 6], X##bo); \
        STORE128(state[ 8], X##bu); \
        STORE128(state[10], X##ga); \
        STORE128(state[12], X##ge); \
        STORE128(state[14], X##gi); \
        STORE128(state[16], X##go); \
        STORE128(state[18], X##gu); \
        STORE128(state[20], X##ka); \
        STORE128(state[22], X##ke); \
        STORE128(state[24], X##ki); \
        STORE128(state[26], X##ko); \
        STORE128(state[28], X##ku); \
        STORE128(state[30], X##ma); \
        STORE128(state[32], X##me); \
        STORE128(state[34], X##mi); \
        STORE128(state[36], X##mo); \
        STORE128(state[38], X##mu); \
        STORE128(state[40], X##sa); \
        STORE128(state[42], X##se); \
        STORE128(state[44], X##si); \
        STORE128(state[46], X##so); \
        STORE128(state[48], X##su); \

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

void KeccakP800times4_PermuteAll_12rounds(void *states) {
  u32 *statesAsLanes = (u32 *)states;
  declareABCDE
  copyFromState(A, &statesAsLanes)
  prepareTheta \
  thetaRhoPiChiIotaPrepareTheta(12, A, E) \
  thetaRhoPiChiIotaPrepareTheta(13, E, A) \
  thetaRhoPiChiIotaPrepareTheta(14, A, E) \
  thetaRhoPiChiIotaPrepareTheta(15, E, A) \
  thetaRhoPiChiIotaPrepareTheta(16, A, E) \
  thetaRhoPiChiIotaPrepareTheta(17, E, A) \
  thetaRhoPiChiIotaPrepareTheta(18, A, E) \
  thetaRhoPiChiIotaPrepareTheta(19, E, A) \
  thetaRhoPiChiIotaPrepareTheta(20, A, E) \
  thetaRhoPiChiIotaPrepareTheta(21, E, A) \
  thetaRhoPiChiIotaPrepareTheta(22, A, E) \
  thetaRhoPiChiIota(23, E, A)
  copyToState(&statesAsLanes, A)
}

void KeccakP800times4_PermuteAll_24rounds(void *states) {

}

void KeccakP800times4_ExtractLanesAll(const void *states, unsigned char *data, unsigned int laneCount, unsigned int laneOffset) {
  u32 *curData0 = (u32 *)data;
  u32 *curData1 = (u32 *)(data+laneOffset*1*SnP_laneLengthInBytes);

  const uint32x4_t *stateAsLanes = (const uint32x4_t *)states;
  const u32 *stateAsLanes32 = (const u32*)states;
  unsigned int i;

  #define Extr( argIndex )    curData0[argIndex] = stateAsLanes32[4*(argIndex)],      \
                              curData1[argIndex] = stateAsLanes32[4*(argIndex)+1]

  for(i=0; i<laneCount; i++)
    Extr( i );
  #undef Extr
}
