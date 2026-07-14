#ifndef _CMT2219A_HAL_H__
#define _CMT2219A_HAL_H__

#include "gpio.h"
#include "board.h"

/* The CMT2219A config registers length, dont't chang it */
#define CMT2219_FTP8_LENGTH							(uint16_t)(0x3E)			// The byte lendth
#define CMT2219_FTP16_LENGTH						(uint16_t)(0x3E/2)		// The word length

//设置为输入
#define CMT2219A_SetGpio1In()            Gpio_InitIO(CMT_GPO1_PORT, CMT_GPO1_PIN, GpioDirIn)
#define CMT2219A_SetGpio2In()            Gpio_InitIO(CMT_GPO2_PORT, CMT_GPO2_PIN, GpioDirIn)
#define CMT2219A_SetGpio3In()            Gpio_InitIO(CMT_GPO3_PORT, CMT_GPO3_PIN, GpioDirIn)
//获取端口状态
#define CMT2219A_ReadGpio1()             Gpio_GetIO(CMT_GPO1_PORT, CMT_GPO1_PIN)
#define CMT2219A_ReadGpio2()             Gpio_GetIO(CMT_GPO2_PORT, CMT_GPO2_PIN)
#define CMT2219A_ReadGpio3()             Gpio_GetIO(CMT_GPO3_PORT, CMT_GPO3_PIN)
//#define CMT2219A_DelayMs(ms)             delay1ms(ms)
//#define CMT2219A_DelayUs(us)             delay1us(us)
//#define CMT2219A_GetTickCount()          g_nSysTickCount


void Cmt2219a_Hal_Init(void);
void Cmt2219a_Hal_Config(void);
uint8_t Radio_Recv_FixedLen(uint8_t *pBuf, uint8_t len);

void Cmt2219a_UpdateConfigs(void);
void Cmt2219a_UpdateRegisters(const uint16_t *buf16);

#endif



