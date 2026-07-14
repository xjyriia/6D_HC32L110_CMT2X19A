#include "REG_CONFIG.h"
#include "cmt2219a.h"
#include "ddl.h"

/* the RFPDK export parameters and make modification as you needed. */ 
const uint16_t g_cmt2219RegConfigs_433[CMT2219_FTP16_LENGTH] = {
	0x2152,		// Frequency	: 433.92 MHz
	0x6666,		// Demodulation	: (G)FSK
	0xF306,		// Xtal Tol.	: 10 ppm
	0xF0A1,		// Symbol Rate	: 2.4 ksps
	0x53BE,		// Deviation	: 23.0 kHz
	0x0001,		// Data Mode    : Packet
	0x1E62,     // Preamble     : 4-Byte
	0x1000,		// Sync         : 0x86868686
	0x2884,		// Node ID      : 0x3A
	0x36E0,		// Data Length  : 4-Byte
	0x8F27,
	0x8686,
	0x8686,
	0x3C00,
	0x3A05,
	0x2145,
	0x8405,
	0x0201,
	0x0019,
	0x0000,
	0x56AC,
	0xD453,
	0x4940,
	0x5DFF,
	0x0012,
	0xFA90,
	0x0000,
	0xC040,
	0x0000,
	0xEB20,
	0x0007,
};

/* the RFPDK export parameters and make modification as you needed. */ 
const uint16_t g_cmt2219RegConfigs_868[CMT2219_FTP16_LENGTH] = {
	0x4272,		// Frequency	: 868.35 MHz
	0x1544,		// Demodulation	: (G)FSK
	0x630D,		// Xtal Tol.	: 10 ppm
	0x809A,		// Symbol Rate	: 9.6 ksps
	0x55C6,		// Deviation	: 35.0 kHz
	0x0000,		// Data Mode    : Packet
	0x2E62,     // Preamble     : 4-Byte
	0x1000,		// Sync         : 0x86868686
	0x1484,		// Node ID      : 0x3A
	0x36E0,		// Data Length  : 4-Byte
	0x8F27,
	0x8686,
	0x8686,
	0x3C00,
	0x3A05,
	0x2145,
	0x8407,
	0x0201,
	0x0019,
	0x0000,
	0xAEAC,
	0xD453,
	0x4940,
	0x59FF,
	0x0012,
	0xFA90,
	0x0000,
	0xC040,
	0x0000,
	0xCA20,
	0x0007,
};

void Cmt2219a_UpdateConfigs(void)
{
	Cmt2219a_GoStby();	// You should go to stby status firstly, when you chang the rf settings.
	delay1ms(1);				// Wait for go to stby status

	// Update the Cmt2219a settings. 
	Cmt2219a_UpdateRegisters(g_cmt2219RegConfigs_433);  // 使用 433MHz 配置
//	Cmt2219a_UpdateRegisters(g_cmt2219RegConfigs_868);
}

void Cmt2219a_UpdateRegisters(const uint16_t *buf16)
{
	uint16_t i;
	static uint8_t buf8[CMT2219_FTP8_LENGTH];

	for(i=0; i<CMT2219_FTP16_LENGTH; i++)
	{
		buf8[2*i]   = (uint8_t)buf16[i];
		buf8[2*i+1] = (uint8_t)(buf16[i]>>8);
	}

	Cmt2219a_WriteNReg(0, buf8, CMT2219_FTP8_LENGTH);
}


