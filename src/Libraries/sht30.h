#ifndef _SHT30_H_
#define _SHT30_H_
#include "common.h"

#define SHT30_ADDR 0x44
#define SHT40_ADDR 0x44
#define AHT20_ADDR 0x38



#define TH_ACC_HIGH 0
#define TH_ACC_MID 1
#define TH_ACC_LOW 2

#define TH_MPS_0_5 0
#define TH_MPS_1 1
#define TH_MPS_2 2
#define TH_MPS_4 3
#define TH_MPS_10 4


//#define TH_Value_float

struct TH_Value
{
#ifdef   TH_Value_float
    float RH;
    float CEL;
#else
    short RH;
    short CEL;
#endif
};
uint8_t SHT40_GetValue_SingleShotWithoutCS(struct TH_Value *value);
uint8_t SHT30_SoftReset(void);
uint8_t AHT20_GetValue_SingleShotWithoutCS(struct TH_Value *value);
uint8_t SHT30_GetValue_SingleShotWithCS(uint8_t acc, struct TH_Value *value);
#endif
