#include "cmt2219a.h"
#include "cmt_spi3.h"

/* Init SPI */
void Cmt2219a_Init(void)
{
	cmt_spi3_init(); 
}


/*! ********************************************************
* @desc:
*     Writing 0xFF into this register will generate a soft_reset command to the device.
*     Setting this register to any values other than 0xFF will not take any effect.
*     The soft reset has the same effect of POR.
*     After the soft reset, the chip will go through the power-up sequence and then enter the SLEEP Mode.
*     After soft resetting the chip, the MCU shall wait 10 ms before performing any other controls to the chip.
* *********************************************************/
void Cmt2219a_SoftReset(void)
{
	cmt_spi3_write(REG_SOFTRST, 0xFF);
} 


/*! ********************************************************
* @desc:
*     Select which signal is assigned to GPO1.
* @param:
*     GPO1_SEL_NRSTO(0): nRSTO is output via GPO1 (default).
*     GPO1_SEL_INT1(1): INT1 is output via GPO1.
*     GPO1_SEL_INT2(2): INT2 is output via GPO1.
*     GPO1_SEL_DOUT(3): DOUT is output via GPO1.
* *********************************************************/
uint8_t Cmt2219a_SetGpo1Sel(uint8_t dat)
{
	// GPO1_SEL: IO_SEL<1:0>

	uint8_t tempdat;

	cmt_spi3_read(REG_IO_SEL, &tempdat);
	tempdat &= ~0x03;
	tempdat |= dat;
	cmt_spi3_write(REG_IO_SEL, tempdat);

	return 0;
}


/*! ********************************************************
* @desc:
*     Select which signal is assigned to GPO2.
* @param:
*     GPO2_SEL_INT1(0): INT1 is output via GPO2 (default).
*     GPO2_SEL_INT2(1): INT2 is output via GPO2.
*     GPO2_SEL_DCLK(2): DCLK is output via GPO2.
*     GPO2_SEL_LOGIC_ZERO(3): logic ¡®0¡¯ is output via GPO2.
* *********************************************************/
uint8_t Cmt2219a_SetGpo2Sel(uint8_t dat)
{
	// GPO2_SEL: IO_SEL<3:2>

	uint8_t tempdat;

	cmt_spi3_read(REG_IO_SEL, &tempdat);
	tempdat &= ~(0x03 << 2);
	tempdat |= (dat << 2);
	cmt_spi3_write(REG_IO_SEL, tempdat);

	return 0;
}


/*! ********************************************************
* @desc:
*     Select which signal is assigned to GPO3.
* @param:
*     GPO3_SEL_CLKO(0): CLKO is output via GPO3 (default).
*     GPO3_SEL_INT1(1): INT1 is output via GPO3.
*     GPO3_SEL_INT2(2): INT2 is output via GPO3.
*     GPO3_SEL_DOUT(3): DOUT is output via GPO3.
* *********************************************************/
uint8_t Cmt2219a_SetGpo3Sel(uint8_t dat)
{
	// GPO3_SEL: IO_SEL<5:4>

	uint8_t tempdat;

	cmt_spi3_read(REG_IO_SEL, &tempdat);
	tempdat &= ~(0x03 << 4);
	tempdat |= (dat << 4);
	cmt_spi3_write(REG_IO_SEL, tempdat);

	return 0;
}


/*! ********************************************************
* @desc:
*     Select which signal is assigned to GPO4.
* @param:
*     GPO4_SEL_DOUT(0): DOUT is output via GPO4 (default).
*     GPO4_SEL_INT1(1): INT1 is output via GPO4.
*     GPO4_SEL_INT2(2): INT2 is output via GPO4.
*     GPO4_SEL_DCLK(3): DCLK is output via GPO4.
* *********************************************************/
uint8_t Cmt2219a_SetGpo4Sel(uint8_t dat)
{
	// GPO4_SEL: IO_SEL<7:6>

	uint8_t tempdat;

	cmt_spi3_read(REG_IO_SEL, &tempdat);
	tempdat &= ~(0x03 << 6);
	tempdat |= (dat << 6);
	cmt_spi3_write(REG_IO_SEL, tempdat);

	return 0;
}


/*! ********************************************************
* @desc:
*     Read out the chip operating state.
* @return:
*     OP_STATUS_PUP(0):    PUP state. 
*     OP_STATUS_SLEEP(1):  SLEEP state (Default). 
*     OP_STATUS_STBY(2):   STBY state. 
*     OP_STATUS_TUNE(3):   TUNE state. 
*     OP_STATUS_RX(4):     RX state. 
*     OP_STATUS_EEPROM(5): EEPROM state.
* *********************************************************/
uint8_t Cmt2219a_GetOpStatus(void)
{
	uint8_t tempdat = 0;

	cmt_spi3_read(REG_OP_CTRL, &tempdat);
	tempdat = tempdat >> 5;

	return tempdat;
}


/*! ********************************************************
* @desc:
*     Switch to EEPROM state.. 
* *********************************************************/
void Cmt2219a_GoEeprom(void)
{
	cmt_spi3_write(REG_OP_CTRL, OP_SWITCH_EEPROM);
}


/*! ********************************************************
* @desc:
*     Switch to STBY state.. 
* *********************************************************/
void Cmt2219a_GoStby(void)
{
	cmt_spi3_write(REG_OP_CTRL, OP_SWITCH_STBY);
}


/*! ********************************************************
* @desc:
*     Switch to TUNE state.. 
* *********************************************************/
void Cmt2219a_GoTune(void)
{
	cmt_spi3_write(REG_OP_CTRL, OP_SWITCH_TUNE);
}


/*! ********************************************************
* @desc:
*     Switch to RX state.. 
* *********************************************************/
void Cmt2219a_GoRx(void)
{
	cmt_spi3_write(REG_OP_CTRL, OP_SWITCH_RX);
}


/*! ********************************************************
* @desc:
*     Switch to SLEEP state.
* *********************************************************/
void Cmt2219a_GoSleep(void)
{
	cmt_spi3_write(REG_OP_CTRL, OP_SWITCH_SLEEP);
}


/*! ********************************************************
* @desc:
*     The value immediately reflects the real time Radio Signal 
*     Strength Indicator (RSSI) when the chip is working in the RX state.
* *********************************************************/
uint8_t Cmt2219a_GetRssiValue(void)
{
	uint8_t tempdat;
	cmt_spi3_read(REG_RSSI, &tempdat);

	return tempdat;
}


/*! ********************************************************
* @desc:
*     The MCU is able to overwrite these 4 registers with 
*     the unique Sync Word at the beginning of the application.
* *********************************************************/
uint8_t Cmt2219a_SetSyncWord(uint32_t dat)
{
	cmt_spi3_write(REG_SYNC_A, (uint8_t)dat);    dat >>= 8;
	cmt_spi3_write(REG_SYNC_B, (uint8_t)dat);    dat >>= 8;
	cmt_spi3_write(REG_SYNC_C, (uint8_t)dat);    dat >>= 8;
	cmt_spi3_write(REG_SYNC_D, (uint8_t)dat);

	return 0;
}


/*! ********************************************************
* @desc:
*     Turn on/off the duty cycle mode. 
* *********************************************************/
uint8_t Cmt2219a_EnableDutyCycle(uint8_t dat)
{
	// DC EN: DC_CTL<7:7>

	uint8_t tempdat;

	cmt_spi3_read(REG_DC_CTL, &tempdat);
	tempdat &= ~(0x01 << 7);
	tempdat |= (dat << 7);
	cmt_spi3_write(REG_DC_CTL, tempdat);

	return 0;
}

uint8_t Cmt2219a_SetLfoscMode(uint8_t dat)
{
	// LFOSC Mode: DC_CTL<6:5>

	uint8_t tempdat;

	cmt_spi3_read(REG_DC_CTL, &tempdat);
	tempdat &= ~(0x03 << 5);
	tempdat |= (dat << 5);
	cmt_spi3_write(REG_DC_CTL, tempdat);

	return 0;
}

/*! ********************************************************
* @desc:
*     Turn on/off the sleep timer. 
* *********************************************************/
uint8_t Cmt2219a_EnableSleepTimer(uint8_t dat)
{
	// SL_CTL: SLP EN<7:7>

	uint8_t tempdat;

	cmt_spi3_read(REG_SL_CTL, &tempdat);
	tempdat &= ~(0x01 << 7);
	tempdat |= (dat << 7);
	cmt_spi3_write(REG_SL_CTL, tempdat);

	return 0;
}

/*! ********************************************************
* @desc:
*     Turn on/off the receive timer. 
* *********************************************************/
uint8_t Cmt2219a_EnableRecvTimer(uint8_t dat)
{
	// RX_CTL: RX EN<7:7>

	uint8_t tempdat;

	cmt_spi3_read(REG_RX_CTL, &tempdat);
	tempdat &= ~(0x01 << 7);
	tempdat |= (dat << 7);
	cmt_spi3_write(REG_RX_CTL, tempdat);

	return 0;
}


/*! ********************************************************
* @param:
*     0: Disable the sleep timer timeout interrupt (default).
*     1: Enable the sleep timer timeout interrupt.
* *********************************************************/
uint8_t Cmt2219a_EnableSleepTimeoutInt(uint8_t dat)
{
	// SL_TMO_EN: INT_EN<7:7>

	uint8_t tempdat;

	cmt_spi3_read(REG_INT_EN, &tempdat);
	tempdat &= ~(0x01 << 7);
	tempdat |= (dat << 7);
	cmt_spi3_write(REG_INT_EN, tempdat);

	return 0;
}


/*! ********************************************************
* @param:
*     0: Disable the receive timer timeout interrupt (default).
*     1: Enable the receive timer timeout interrupt.
* *********************************************************/
uint8_t Cmt2219a_EnableRecvTimeoutInt(uint8_t dat)
{
	// RX_TMO_EN: INT_EN<6:6>

	uint8_t tempdat;

	cmt_spi3_read(REG_INT_EN, &tempdat);
	tempdat &= ~(0x01 << 6);
	tempdat |= (dat << 6);
	cmt_spi3_write(REG_INT_EN, tempdat);

	return 0;
}


/*! ********************************************************
* @param:
*     0: Disable the RSSI valid interrupt (default).
*     1: Enable the RSSI valid interrupt.
* *********************************************************/
uint8_t Cmt2219a_EnableRssiValidInt(uint8_t dat)
{
	// RSSI_VLD_EN: INT_EN<5:5>

	uint8_t tempdat;

	cmt_spi3_read(REG_INT_EN, &tempdat);
	tempdat &= ~(0x01 << 5);
	tempdat |= (dat << 5);
	cmt_spi3_write(REG_INT_EN, tempdat);

	return 0;
}


/*! ********************************************************
* @param:
*     0: Disable the preamble detection pass interrupt (default).
*     1: Enable the preamble detection pass interrupt.
* *********************************************************/
uint8_t Cmt2219a_EnablePreamblePassInt(uint8_t dat)
{
	// PREAM_PS_EN: INT_EN<4:4>

	uint8_t tempdat;

	cmt_spi3_read(REG_INT_EN, &tempdat);
	tempdat &= ~(0x01 << 4);
	tempdat |= (dat << 4);
	cmt_spi3_write(REG_INT_EN, tempdat);

	return 0;
}


/*! ********************************************************
* @param:
*     0: Disable the sync word detection pass interrupt (default).
*     1: Enable the sync word detection pass interrupt.
* *********************************************************/
uint8_t Cmt2219a_EnableSyncWordPassInt(uint8_t dat)
{
	// SYNC_PS_EN: INT_EN<3:3>

	uint8_t tempdat;

	cmt_spi3_read(REG_INT_EN, &tempdat);
	tempdat &= ~(0x01 << 3);
	tempdat |= (dat << 3);
	cmt_spi3_write(REG_INT_EN, tempdat);

	return 0;
}


/*! ********************************************************
* @param:
*     0: Disable the node ID detection pass interrupt (default).
*     1: Enable the node ID detection pass interrupt.
* *********************************************************/
uint8_t Cmt2219a_EnableNodeIdPassInt(uint8_t dat)
{
	// NODE_PS_EN: INT_EN<2:2>

	uint8_t tempdat;

	cmt_spi3_read(REG_INT_EN, &tempdat);
	tempdat &= ~(0x01 << 2);
	tempdat |= (dat << 2);
	cmt_spi3_write(REG_INT_EN, tempdat);

	return 0;
}


/*! ********************************************************
* @param:
*     0: Disable the CRC validation pass interrupt (default).
*     1: Enable the CRC validation pass interrupt.
* *********************************************************/
uint8_t Cmt2219a_EnableCrcPassInt(uint8_t dat)
{
	// CRC_PS_EN: INT_EN<1:1>

	uint8_t tempdat;

	cmt_spi3_read(REG_INT_EN, &tempdat);
	tempdat &= ~(0x01 << 1);
	tempdat |= (dat << 1);
	cmt_spi3_write(REG_INT_EN, tempdat);

	return 0;
}


/*! ********************************************************
* @param:
*     0: Disable the packet receive finish interrupt (default).
*     1: Enable the packet receive finish interrupt.
* *********************************************************/
uint8_t Cmt2219a_EnablePacketDoneInt(uint8_t dat)
{
	// PKT_DONE_EN: INT_EN<0:0>

	uint8_t tempdat;

	cmt_spi3_read(REG_INT_EN, &tempdat);
	tempdat &= ~0x01;
	tempdat |= dat;
	cmt_spi3_write(REG_INT_EN, tempdat);

	return 0;
}


/*! ********************************************************
* @desc:
*     Select which interrupt is observable on the INT2.
* *********************************************************/
uint8_t Cmt2219a_SetInt1(uint8_t dat)
{
	// INT1_CTL: INTCTL_A<3:0>

	uint8_t tempdat;

	cmt_spi3_read(REG_INTCTL_A, &tempdat);
	tempdat &= ~0x0F;
	tempdat |= dat;
	cmt_spi3_write(REG_INTCTL_A, tempdat);

	return 0;
}


/*! ********************************************************
* @desc:
*     Select which interrupt is observable on the INT1.
* *********************************************************/
uint8_t Cmt2219a_SetInt2(uint8_t dat)
{
	// INT2_CTL: INTCTL_A<7:4>

	uint8_t tempdat;

	cmt_spi3_read(REG_INTCTL_A, &tempdat);
	tempdat &= ~(0x0F << 4);
	tempdat |= (dat << 4);
	cmt_spi3_write(REG_INTCTL_A, tempdat);

	return 0;
}


/*! ********************************************************
* @param:
*     0: Keep the sleep timer timeout interrupt (default). 
*     1: Clear the sleep timer timeout interrupt.
* *********************************************************/
uint8_t Cmt2219a_ClearSleepTimeoutInt(uint8_t dat)
{
	// SL_TMO_CLR: INTCTL_B<7:7>

	uint8_t tempdat;

	cmt_spi3_read(REG_INTCTL_B, &tempdat);
	tempdat &= ~(0x01 << 7);
	tempdat |= (dat << 7);
	cmt_spi3_write(REG_INTCTL_B, tempdat);

	return 0;
}


/*! ********************************************************
* @param:
*     0: Keep the receive timer timeout interrupt (default).
*     1: Clear the receive timer timeout interrupt. 
* *********************************************************/
uint8_t Cmt2219a_ClearRecvTimeoutInt(uint8_t dat)
{
	// RX_TMO_CLR: INTCTL_B<6:6>

	uint8_t tempdat;

	cmt_spi3_read(REG_INTCTL_B, &tempdat);
	tempdat &= ~(0x01 << 6);
	tempdat |= (dat << 6);
	cmt_spi3_write(REG_INTCTL_B, tempdat);

	return 0;
}


/*! ********************************************************
* @param:
*     0: Keep the RSSI valid interrupt (default).
*     1: Clear the RSSI valid interrupt. 
* *********************************************************/
uint8_t Cmt2219a_ClearRssiValidInt(uint8_t dat)
{
	// RSSI_VLD_CLR: INTCTL_B<5:5>

	uint8_t tempdat;

	cmt_spi3_read(REG_INTCTL_B, &tempdat);
	tempdat &= ~(0x01 << 5);
	tempdat |= (dat << 5);
	cmt_spi3_write(REG_INTCTL_B, tempdat);

	return 0;
}


/*! ********************************************************
* @param:
*     0: Keep the preamble detection pass interrupt (default).
*     1: Clear the preamble detection pass interrupt. 
* *********************************************************/
uint8_t Cmt2219a_ClearPreamblePassInt(uint8_t dat)
{
	// PREAM_PS_CLR: INTCTL_B<4:4>

	uint8_t tempdat;

	cmt_spi3_read(REG_INTCTL_B, &tempdat);
	tempdat &= ~(0x01 << 4);
	tempdat |= (dat << 4);
	cmt_spi3_write(REG_INTCTL_B, tempdat);

	return 0;
}


/*! ********************************************************
* @param:
*     0: Keep the sync word detection pass interrupt (default).
*     1: Clear the sync word detection pass interrupt. 
* *********************************************************/
uint8_t Cmt2219a_ClearSyncWordPassInt(uint8_t dat)
{
	// SYNC_PS_CLR: INTCTL_B<3:3>

	uint8_t tempdat;

	cmt_spi3_read(REG_INTCTL_B, &tempdat);
	tempdat &= ~(0x01 << 3);
	tempdat |= (dat << 3);
	cmt_spi3_write(REG_INTCTL_B, tempdat);

	return 0;
}


/*! ********************************************************
* @param:
*     0: Keep the node ID detection pass interrupt (default).
*     1: Clear the node ID detection pass interrupt. 
* *********************************************************/
uint8_t Cmt2219a_ClearNodeIdPassInt(uint8_t dat)
{
	// NODE_PS_CLR: INTCTL_B<2:2>

	uint8_t tempdat;

	cmt_spi3_read(REG_INTCTL_B, &tempdat);
	tempdat &= ~(0x01 << 2);
	tempdat |= (dat << 2);
	cmt_spi3_write(REG_INTCTL_B, tempdat);

	return 0;
}


/*! ********************************************************
* @param:
*     0: Keep the CRC validation pass interrupt (default).
*     1: Clear the CRC validation pass interrupt. 
* *********************************************************/
uint8_t Cmt2219a_ClearCrcPassInt(uint8_t dat)
{
	// CRC_PS_CLR: INTCTL_B<1:1>

	uint8_t tempdat;

	cmt_spi3_read(REG_INTCTL_B, &tempdat);
	tempdat &= ~(0x01 << 1);
	tempdat |= (dat << 1);
	cmt_spi3_write(REG_INTCTL_B, tempdat);

	return 0;
}


/*! ********************************************************
* @param:
*     0: Keep the packet receive finish interrupt (default).
*     1: Clear the packet receive finish interrupt. 
* *********************************************************/
uint8_t Cmt2219a_ClearPacketDoneInt(uint8_t dat)
{
	// PKT_DONE_CLR: INTCTL_B<0:0>

	uint8_t tempdat;

	cmt_spi3_read(REG_INTCTL_B, &tempdat);
	tempdat &= ~0x01;
	tempdat |= dat;
	cmt_spi3_write(REG_INTCTL_B, tempdat);

	return 0;
}


/*! ********************************************************
* @desc:
*     This interrupt is generated when the sleep timer 
*     is turned on and the SL_TMO_EN is set to 1.
*     The flag goes high at the sleep timer timeout. 
*     It can be cleared by setting the SL_TMO_CLR to 1.
* *********************************************************/
uint8_t Cmt2219a_GetSleepTimeoutFlag(void) 
{
	// SL_TMO_FLG: INTCTL_C<7:7>

	uint8_t tempdat;

	cmt_spi3_read(REG_INTCTL_C, &tempdat);
	return ( (tempdat >> 7) & 0x01 );
}


/*! ********************************************************
* @desc:
*     This interrupt is generated when the receive timer 
*     is turned on and the RX_TMO_EN is set to 1.
*     The flag goes high at the receive timer timeout. 
*     It can be cleared by setting the RX_TMO_CLR to 1.
* *********************************************************/
uint8_t Cmt2219a_GetRecvTimeoutFlag(void)
{
	// RX_TMO_FLG: INTCTL_C<6:6>

	uint8_t tempdat;

	cmt_spi3_read(REG_INTCTL_C, &tempdat);
	return ( (tempdat >> 6) & 0x01 );
}


/*! ********************************************************
* @desc:
*     This interrupt is generated when (G)FSK demodulation 
*     is used and the RSSI_VLD_EN is set to 1.
*     The flag goes high when the RSSI exceeds the FSK Trigger Threshold. 
*     It can be cleared by setting the RSSI_VLD_CLR to 1.
* *********************************************************/
uint8_t Cmt2219a_GetRssiValidFlag(void)
{
	// RSSI_VLD_FLG: INTCTL_C<5:5>

	uint8_t tempdat;

	cmt_spi3_read(REG_INTCTL_C, &tempdat);
	return ( (tempdat >> 5) & 0x01 );
}


/*! ********************************************************
* @desc:
*     This interrupt is generated when the preamble validation 
*     is passed and the PREAM_PS_EN is set to 1.
*     It can be cleared by setting the PREAM_PS_CLR to 1.
* *********************************************************/
uint8_t Cmt2219a_GetPreamblePassFlag(void)
{
	// PREAM_PS_FLG: INTCTL_C<4:4>

	uint8_t tempdat;

	cmt_spi3_read(REG_INTCTL_C, &tempdat);
	return ( (tempdat >> 4) & 0x01 );
}


/*! ********************************************************
* @desc:
*     This interrupt is generated when the sync word validation 
*     is passed and the SYNC_PS_EN is set to 1.
*     It can be cleared by setting the SYNC_PS_CLR to 1.
* *********************************************************/
uint8_t Cmt2219a_GetSyncWordPassFlag(void)
{
	// SYNC_PS_FLG: INTCTL_C<3:3>

	uint8_t tempdat;

	cmt_spi3_read(REG_INTCTL_C, &tempdat);
	return ( (tempdat >> 3) & 0x01 );
}


/*! ********************************************************
* @desc:
*     This interrupt is generated when the node ID validation 
*     is passed in the packet mode and the NODE_PS_EN is set to 1.
*     It can be cleared by setting the NODE_PS_CLR to 1.
* *********************************************************/
uint8_t Cmt2219a_GetNodeIdPassFlag(void)
{
	// NODE_PS_FLG: INTCTL_C<2:2>

	uint8_t tempdat;

	cmt_spi3_read(REG_INTCTL_C, &tempdat);
	return ( (tempdat >> 2) & 0x01 );
}


/*! ********************************************************
* @desc:
*     This interrupt is generated when the CRC validation 
*     is passed in the packet mode and the CRC_PS_EN is set to 1.
*     It can be cleared by setting the CRC_PS_CLR to 1.
* *********************************************************/
uint8_t Cmt2219a_GetCrcPassFlag(void)
{
	// CRC_PS_FLG: INTCTL_C<1:1>

	uint8_t tempdat;

	cmt_spi3_read(REG_INTCTL_C, &tempdat);
	return ( (tempdat >> 1) & 0x01 );
}


/*! ********************************************************
* @desc:
*     This interrupt is generated when a packet is received (regardless of 
*     the CRC validation) in the packet mode and the PKT_DONE_EN is set to 1.
*     It can be cleared by setting the PKT_DONE_CLR to 1.
* *********************************************************/
uint8_t Cmt2219a_GetPacketDoneFlag(void)
{
	// PKT_DONE_FLG: INTCTL_C<0:0>

	uint8_t tempdat;

	cmt_spi3_read(REG_INTCTL_C, &tempdat);
	return ( tempdat & 0x01 );
}


/*! ********************************************************
* @desc:
*     This interrupt is generated in the FIFO Mode and Packet Mode. 
*     The flag goes high when the FIFO is full.  
*     It is cleared automatically when the FIFO is not full. 
* *********************************************************/
uint8_t Cmt2219a_GetFifoFullFlag(void)
{
	// FIFO_FULL_FLG: INTCTL_D<6:6>

	uint8_t tempdat;

	cmt_spi3_read(REG_INTCTL_D, &tempdat);
	return ( (tempdat >> 6) & 0x01 );
}


/*! ********************************************************
* @desc:
*     This interrupt is generated in the FIFO Mode and Packet Mode. 
*     The flag goes high when the FIFO is found to be not empty 
*     (the number of unread data byte is not zero).  
*     It is cleared automatically when the FIFO is empty. 
* *********************************************************/
uint8_t Cmt2219a_GetFifoNotEmptyFlag(void)
{
	// FIFO_NMTY_FLG: INTCTL_D<5:5>

	uint8_t tempdat;

	cmt_spi3_read(REG_INTCTL_D, &tempdat);
	return ( (tempdat >> 5) & 0x01 );
}


/*! ********************************************************
* @desc:
*     This interrupt is generated in the FIFO Mode and Packet Mode. 
*     The flag goes high when the number of unread data bytes 
*     reaches/exceeds the value defined in the ¡°FIFO Threshold¡± parameter.  
*     It is cleared automatically when the number of unread 
*     data bytes is less than the ¡°FIFO Threshold¡±. 
* *********************************************************/
uint8_t Cmt2219a_GetFifoThresholdFlag(void)
{
	// FIFO_TH_FLG: INTCTL_D<4:4>

	uint8_t tempdat;

	cmt_spi3_read(REG_INTCTL_D, &tempdat);
	return ( (tempdat >> 4) & 0x01 );
}


/*! ********************************************************
* @desc:
*     This interrupt is generated in the FIFO Mode and Packet Mode. 
*     The flag goes high when the FIFO is overflow.  
*     It is cleared automatically when the FIFO is not overflow. 
* *********************************************************/
uint8_t Cmt2219a_GetFifoOverflowFlag(void)
{
	// FIFO_OVF_FLG: INTCTL_D<3:3>

	uint8_t tempdat;

	cmt_spi3_read(REG_INTCTL_D, &tempdat);
	return ( (tempdat >> 3) & 0x01 );
}


/*! ********************************************************
* @desc:
*     Note: This bit only takes effect in the STBY, TUNE, RX and EEPROM state.
*     In the SLEEP state, the FIFO and packet handler is automatically cleared and reset.
* @param:
*     0: Keep the FIFO content (default).  
*     1: Clear the FIFO content. 
* *********************************************************/
uint8_t Cmt2219a_ClearFifoContent(uint8_t dat)
{
	// FIFO_PKT_CLR: INTCTL_D<2:2>

	uint8_t tempdat;

	cmt_spi3_read(REG_INTCTL_D, &tempdat);
	tempdat &= ~(0x01 << 2);
	tempdat |= (dat << 2);
	cmt_spi3_write(REG_INTCTL_D, tempdat);

	return 0;
}


/*! ********************************************************
* @desc: 
*     During manufacturing stage, a few steps are required to 
*     perform to enter the test mode.
*     Stopping the running sleep timer is one of those steps 
*     (see Chapter Manufacturing Test for details).
* @param:
*     0: Do not disturb the sleep timer (default).
*     1: Stop the sleep timer.
* *********************************************************/
uint8_t Cmt2219a_StopSleepTimer(uint8_t dat)
{
	// SLTIMER_STOP: FUNC_EN<6:6>

	uint8_t tempdat;

	cmt_spi3_read(REG_FUNC_EN, &tempdat);
	tempdat &= ~(0x01 << 6);
	tempdat |= (dat << 6);
	cmt_spi3_write(REG_FUNC_EN, tempdat);

	return 0;
}


/*! ********************************************************
* @desc: 
*     During manufacturing stage, a few steps are required to 
*     perform to enter the test mode.
*     Setting this bit to 1 is one of those steps 
*     (see Chapter Manufacturing Test for details).
* @param:
*     0: Disable the manufacturing test mode (default).
*     1: Enable the manufacturing test mode.
* *********************************************************/
uint8_t Cmt2219a_EnableManuTestMode(uint8_t dat)
{
	// PROD_TEST_EN: FUNC_EN<5:5>

	uint8_t tempdat;

	cmt_spi3_read(REG_FUNC_EN, &tempdat);
	tempdat &= ~(0x01 << 5);
	tempdat |= (dat << 5);
	cmt_spi3_write(REG_FUNC_EN, tempdat);

	return 0;
}


/*! ********************************************************
* @desc: 
*     To minimize the power consumption the maximum SPI clock speed is 400 kHz.
*     The user can used this bit to allow up to 1MHz SPI speed supported.
* @param:
*     0: Support maximum SPI clock speed of 400 kHz (default).
*     1: Support maximum SPI clock speed of 1 MHz.
* *********************************************************/
uint8_t Cmt2219a_EnableHighSpeedSpi(uint8_t dat)
{
	// HS_SPI_EN: FUNC_EN<2:2>

	uint8_t tempdat;

	cmt_spi3_read(REG_FUNC_EN, &tempdat);
	tempdat &= ~(0x01 << 2);
	tempdat |= (dat << 2);
	cmt_spi3_write(REG_FUNC_EN, tempdat);

	return 0;
}

uint8_t Cmt2219a_ReadReg(uint8_t addr)
{
	uint8_t value;
	cmt_spi3_read(addr, &value);
	return value;
}

uint8_t Cmt2219a_WriteReg(uint8_t addr, uint8_t value)
{
	cmt_spi3_write(addr, value);
	return 0;
}

uint8_t Cmt2219a_WriteNReg(uint8_t addr, uint8_t *dat, uint8_t len)
{
	uint8_t i, tempdat;

	for (i = 0; i < len; i++) 
	{
		cmt_spi3_write(addr, dat[i]);
		cmt_spi3_read(addr, &tempdat);
		if (dat[i] != tempdat)
			return 1;
		addr++;
	}

	return 0;
}

uint8_t Cmt2219a_ReadNReg(uint8_t addr, uint8_t *dat, uint8_t len)
{
	uint8_t tempdat, i;
 
	for (i = 0; i < len; i++) 
	{
		cmt_spi3_read(addr, &tempdat);
		dat[i] = tempdat;
		addr++;
	}

	return 0;
}


