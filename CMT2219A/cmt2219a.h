/*
 * THE FOLLOWING FIRMWARE IS PROVIDED: (1) "AS IS" WITH NO WARRANTY; AND
 * (2)TO ENABLE ACCESS TO CODING INFORMATION TO GUIDE AND FACILITATE CUSTOMER.
 * CONSEQUENTLY, CMOSTEK SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT OR
 * CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
 * OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
 * CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * Copyright (C) CMOSTEK SZ.
 */

/*!
 * @file    CMT2219A.h
 * @brief   CMT2219A transmitter RF chip driver
 *
 * @version 1.0
 * @date    Mar 17 2015
 * @author  CMOSTEK R@D
 */

#ifndef __CMT2219A_H
#define __CMT2219A_H

#include <stdint.h>

/* Sync word registers address */
#define REG_SYNC_A               0x16
#define REG_SYNC_B               0x17
#define REG_SYNC_C               0x18
#define REG_SYNC_D               0x19
#define REG_DC_CTL               0x1F
#define REG_SL_CTL               0x23
#define REG_RX_CTL               0x25

/* Control bank registers address */ 
#define REG_INT_EN               0x3F
#define REG_IO_SEL               0x40
#define REG_INTCTL_A             0x41
#define REG_INTCTL_B             0x42
#define REG_INTCTL_C             0x43
#define REG_INTCTL_D             0x44
#define REG_RSSI                 0x45
#define REG_FUNC_EN              0x46
#define REG_OP_CTRL              0x47
#define REG_SOFTRST              0x4F 

/* Define those signals can be assigned to GPO4 */
#define GPO4_SEL_DOUT            0
#define GPO4_SEL_INT1            1
#define GPO4_SEL_INT2            2
#define GPO4_SEL_DCLK            3

/* Define those signals can be assigned to GPO3 */
#define GPO3_SEL_CLKO            0
#define GPO3_SEL_INT1            1
#define GPO3_SEL_INT2            2
#define GPO3_SEL_DOUT            3

/* Define those signals can be assigned to GPO2 */
#define GPO2_SEL_INT1            0
#define GPO2_SEL_INT2            1
#define GPO2_SEL_DCLK            2
#define GPO2_SEL_LOGIC_ZERO      3

/* Define those signals can be assigned to GPO1 */
#define GPO1_SEL_NRSTO           0
#define GPO1_SEL_INT1            1
#define GPO1_SEL_INT2            2
#define GPO1_SEL_DOUT            3

/* Define the interrupts of direct mode */
#define DIRECT_RSSI_VLD          1
#define DIRECT_PREAM_PS          2
#define DIRECT_SYNC_PS           3
#define DIRECT_SL_TMO            4
#define DIRECT_RX_TMO            5
#define DIRECT_RSSI_INDI         6

/* Define the interrupts of buffer mode */
#define BUFFER_RSSI_VLD          1
#define BUFFER_PREAM_PS          2
#define BUFFER_SYNC_PS           3
#define BUFFER_SL_TMO            4
#define BUFFER_RX_TMO            5
#define BUFFER_FIFO_NMTY         6
#define BUFFER_FIFO_TH           7
#define BUFFER_FIFO_FULL         8
#define BUFFER_FIFO_WBYTE        9
#define BUFFER_FIFO_OVF          10
#define BUFFER_RSSI_INDI         11

/* Define the interrupts of packet mode */
#define PACKET_RSSI_VLD          1
#define PACKET_PREAM_PS          2
#define PACKET_SYNC_PS           3
#define PACKET_NODE_PS           4
#define PACKET_CRC_PS            5
#define PACKET_PKT_DONE          6
#define PACKET_SL_TMO            7
#define PACKET_RX_TMO            8
#define PACKET_FIFO_NMTY         9
#define PACKET_FIFO_TH           10
#define PACKET_FIFO_FULL         11
#define PACKET_FIFO_WBYTE        12
#define PACKET_FIFO_OVF          13
#define PACKET_RSSI_INDI         14

/* Define the chip operating states */
#define OP_STATUS_PUP            0
#define OP_STATUS_SLEEP          1
#define OP_STATUS_STBY           2
#define OP_STATUS_TUNE           3
#define OP_STATUS_RX             4
#define OP_STATUS_EEPROM         5

/* Switch the chip operating states */
#define OP_SWITCH_EEPROM         0x01
#define OP_SWITCH_STBY           0x02
#define OP_SWITCH_TUNE           0x04
#define OP_SWITCH_RX             0x08
#define OP_SWITCH_SLEEP          0x10

/* Define the chip data mode */
#define DIRECT_MODE              0
#define BUFFER_MODE              1
#define PACKET_MODE              2 

 

void Cmt2219a_Init(void);
void Cmt2219a_SoftReset(void); 

/*Select which signal(INT1,INT2,DOUT etc.) is assigned to GPO1/2/3/4 */
uint8_t Cmt2219a_SetGpo1Sel(uint8_t dat);
uint8_t Cmt2219a_SetGpo2Sel(uint8_t dat);
uint8_t Cmt2219a_SetGpo3Sel(uint8_t dat);
uint8_t Cmt2219a_SetGpo4Sel(uint8_t dat);

/* Switch to EEPROM/STBY/TUNE/RX/SLEEP state */
void Cmt2219a_GoEeprom(void);
void Cmt2219a_GoStby(void);
void Cmt2219a_GoTune(void);
void Cmt2219a_GoRx(void);
void Cmt2219a_GoSleep(void);

/* Get the current operating state of the CMT2219A */
uint8_t Cmt2219a_GetOpStatus(void);

/* Get the current RSSI value */
uint8_t Cmt2219a_GetRssiValue(void);

/* Set the sync word of the CMT2219A(Buffer and Packet mode) */
uint8_t Cmt2219a_SetSyncWord(uint32_t dat);

/* Turn on/off duty cycle mode */
uint8_t Cmt2219a_EnableDutyCycle(uint8_t dat);
uint8_t Cmt2219a_SetLfoscMode(uint8_t dat);

/* Turn on/off sleep timer */
uint8_t Cmt2219a_EnableSleepTimer(uint8_t dat);

/* Turn on/off receiver timer */
uint8_t Cmt2219a_EnableRecvTimer(uint8_t dat);

/* Select which interrupt is observable on the INT1/INT2 */
uint8_t Cmt2219a_SetInt1(uint8_t dat);
uint8_t Cmt2219a_SetInt2(uint8_t dat);

/* Enable interrupt */
uint8_t Cmt2219a_EnableSleepTimeoutInt(uint8_t dat);
uint8_t Cmt2219a_EnableRecvTimeoutInt(uint8_t dat);
uint8_t Cmt2219a_EnableRssiValidInt(uint8_t dat);
uint8_t Cmt2219a_EnablePreamblePassInt(uint8_t dat);
uint8_t Cmt2219a_EnableSyncWordPassInt(uint8_t dat);
uint8_t Cmt2219a_EnableNodeIdPassInt(uint8_t dat);
uint8_t Cmt2219a_EnableCrcPassInt(uint8_t dat);
uint8_t Cmt2219a_EnablePacketDoneInt(uint8_t dat);

/* Clear the interrupt flag */
uint8_t Cmt2219a_ClearSleepTimeoutInt(uint8_t dat);
uint8_t Cmt2219a_ClearRecvTimeoutInt(uint8_t dat);
uint8_t Cmt2219a_ClearRssiValidInt(uint8_t dat);
uint8_t Cmt2219a_ClearPreamblePassInt(uint8_t dat);
uint8_t Cmt2219a_ClearSyncWordPassInt(uint8_t dat);
uint8_t Cmt2219a_ClearNodeIdPassInt(uint8_t dat);
uint8_t Cmt2219a_ClearCrcPassInt(uint8_t dat);
uint8_t Cmt2219a_ClearPacketDoneInt(uint8_t dat);

/* Get the interrupt flag */
uint8_t Cmt2219a_GetSleepTimeoutFlag(void);
uint8_t Cmt2219a_GetRecvTimeoutFlag(void);
uint8_t Cmt2219a_GetRssiValidFlag(void);
uint8_t Cmt2219a_GetPreamblePassFlag(void);
uint8_t Cmt2219a_GetSyncWordPassFlag(void);
uint8_t Cmt2219a_GetNodeIdPassFlag(void);
uint8_t Cmt2219a_GetCrcPassFlag(void);
uint8_t Cmt2219a_GetPacketDoneFlag(void);

uint8_t Cmt2219a_GetFifoFullFlag(void);
uint8_t Cmt2219a_GetFifoNotEmptyFlag(void);
uint8_t Cmt2219a_GetFifoThresholdFlag(void);
uint8_t Cmt2219a_GetFifoOverflowFlag(void); 
uint8_t Cmt2219a_ClearFifoContent(uint8_t dat);

uint8_t Cmt2219a_StopSleepTimer(uint8_t dat);
uint8_t Cmt2219a_EnableManuTestMode(uint8_t dat);
uint8_t Cmt2219a_EnableHighSpeedSpi(uint8_t dat);

uint8_t Cmt2219a_ReadReg(uint8_t addr);
uint8_t Cmt2219a_WriteReg(uint8_t addr, uint8_t value);

uint8_t Cmt2219a_WriteNReg(uint8_t addr, uint8_t *dat, uint8_t len);
uint8_t Cmt2219a_ReadNReg(uint8_t addr, uint8_t *dat, uint8_t len);


#endif //__CMT2219A_H__
