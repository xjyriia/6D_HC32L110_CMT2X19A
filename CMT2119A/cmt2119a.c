#include "cmt2119a.h"
#include "rfm_twi.h"
#include "gpio.h"

void CMT2119A_TwiReset(void)  //Step1
{
		TWI_RST();
}

void CMT2119A_SoftReset(void)  //Step2
{
		SOFT_RST();
}

void CMT2119A_OpenLdoAndOsc(void)  //Step3
{
		TWI_WRREG(0x02, 0x78);
}

void CMT2119A_ActiveRegsister(void)  //Step4
{
		TWI_WRREG(0x2F, 0x80);
		TWI_WRREG(0x35, 0xCA);
		TWI_WRREG(0x36, 0xEB);
		TWI_WRREG(0x37, 0x37);
		TWI_WRREG(0x38, 0x82);
}	
	
void CMT2119A_EnableRegMode(void)  //Step5
{
		TWI_WRREG(0x12, 0x10);
		TWI_WRREG(0x12, 0x00);
		TWI_WRREG(0x24, 0x07);
		TWI_WRREG(0x1D, 0x20);
}

void CMT2119A_WriteReg(uint8_t add, uint16_t dat)  //Step6
{
		TWI_WRREG(0x18, add);
		TWI_WRREG(0x19, (uint8_t)dat);
		TWI_WRREG(0x1A, (uint8_t)(dat>>8));
		TWI_WRREG(0x25, 0x01);
}

uint16_t CMT2119A_ReadReg(uint8_t add)
{
		uint8_t low_dat,high_dat;
		uint16_t dat;
	
		TWI_WRREG(0x18, add);
		TWI_RDREG(0x1B, &low_dat);
		TWI_RDREG(0x1C, &high_dat);
		dat = ((uint16_t)high_dat<<8)|low_dat;
		return dat;
}

void CMT2119A_TwiOff(void)  //Step7
{
		TWI_OFF();
}

void CMT2119A_Transmit(uint8_t pBuf[], uint8_t len)  //Step8
{
  Transimission(pBuf,len);
}

void CMT2119A_CloseLdoAndOsc(void)  //Step9
{
 	  TWI_WRREG(0x02, 0x7F);
}

bool CMT2119A_Init(const uint16_t para[],uint8_t length)  //SoftwareInit of chip
{
		uint8_t i;	

		TWI_Init();	

		CMT2119A_TwiReset();						//step 1
		CMT2119A_SoftReset();						//step 2
		delay1ms(2);										//wait more than 1ms		
		CMT2119A_OpenLdoAndOsc();				//step 3
		CMT2119A_ActiveRegsister();			//step 4
		CMT2119A_EnableRegMode();				//step 5
		for(i=0;i<length;i++)						//step 6
		{
			CMT2119A_WriteReg(i, para[i]);
		}		
		if(FALSE==CMT2119A_IsExist())
			return 0;
		
		CMT2119A_TwiOff();							//step 7	
		rfm_twi_data_0();          			//Set Dat to low
		rfm_twi_data_out();							//Read for Tx
		return 1;
}

void CMT2119A_Sleep(void)
{
		TWI_Init();											// 初始化 TWI GPIO
		CMT2119A_TwiReset();						// TWI 复位
		CMT2119A_CloseLdoAndOsc();			// 关闭 LDO 和振荡器
		CMT2119A_TwiOff();							// 关闭 TWI
		
		CMT2119A_TwiReset();						//step 1
		CMT2119A_SoftReset();						//step 2
		// GPIO 由 user_enter_deep_sleep_mode 统一配置
}

/**
 * @brief  CMT2119A 快速唤醒 (从 Sleep 状态恢复)
 * @note   Sleep 只关闭 LDO，寄存器配置保留，无需重新写入
 */
void CMT2119A_Wakeup(void)
{
		TWI_Init();											// 重新初始化 TWI GPIO
		CMT2119A_TwiReset();						// TWI 复位
		delay1ms(1);										// 等待稳定
		CMT2119A_OpenLdoAndOsc();				// 打开 LDO 和振荡器
		delay1ms(1);										// 等待 LDO 稳定
		CMT2119A_TwiOff();							// 关闭 TWI
		rfm_twi_data_0();								// DAT 置低
		rfm_twi_data_out();							// 设置为输出
}

bool CMT2119A_IsExist(void)
{
		uint16_t back, dat;
	
    back = CMT2119A_ReadReg(0x00);
    CMT2119A_WriteReg(0x00, 0x0000);

    dat = CMT2119A_ReadReg(0x00);
    CMT2119A_WriteReg(0x00, back);

    if(0x0000==dat)
        return TRUE;
    else
        return FALSE;
}


