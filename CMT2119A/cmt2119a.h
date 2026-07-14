#ifndef __CMT2119A_H__
#define __CMT2119A_H__

#include "gpio.h"
#include <stdbool.h>
#include <stdint.h>

void CMT2119A_TwiReset(void);
void CMT2119A_SoftReset(void);
void CMT2119A_OpenLdoAndOsc(void);
void CMT2119A_ActiveRegsister(void);
void CMT2119A_EnableRegMode(void);
void CMT2119A_WriteReg(uint8_t add, uint16_t dat);
uint16_t CMT2119A_ReadReg(uint8_t add);
void CMT2119A_TwiOff(void);
void CMT2119A_Transmit(uint8_t pBuf[], uint8_t len);
void CMT2119A_CloseLdoAndOsc(void);

bool CMT2119A_Init(const uint16_t para[],uint8_t length);
void CMT2119A_Sleep(void);
void CMT2119A_Wakeup(void);  // 快速唤醒 (从 Sleep 恢复)
bool CMT2119A_IsExist(void);

#endif

