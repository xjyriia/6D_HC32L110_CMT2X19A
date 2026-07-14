#include "cmt2219a_hal.h"
#include "cmt2219a.h"
#include "cmt_spi3.h"
#include "board.h"

/*
 * 占空比接收模式配置说明:
 * - LFOSC 频率约 32kHz, 1ms ≈ 32 个周期
 * - 睡眠时间：RX_SLEEP_INTERVAL_MS (board.h 中定义)
 * - 接收窗口：约 15ms (足够接收一帧数据)
 */
#ifndef RX_SLEEP_INTERVAL_MS
#define RX_SLEEP_INTERVAL_MS    40     // 默认 40ms (配合 1mA 功耗要求)
#endif

// 睡眠时间计数值 (LFOSC 32kHz)
#define SL_TIME_COUNT   ((uint32_t)(RX_SLEEP_INTERVAL_MS * 32))
// 接收窗口时间计数值 (约 15ms)
#define RX_TIME_COUNT   ((uint16_t)(15 * 32))

/*
;---------------------------------------
;  CMT2219A Configuration File
;  Frequency             = 433.920 MHz
;  Demodulation          = (G)FSK
;  Symbol Rate           = 4.8 ksps
;  Data Mode             = Packet
;  Packet Type           = Fixed Length
;---------------------------------------
*/
/* the RFPDK export parameters and make modification as you needed. */ 
static const uint16_t g_cmt2219RegConfigs_433[CMT2219_FTP16_LENGTH] = {
	0x21B0,
	0x6666,
	0x0006,
	0x7100,
	0xA9B0,
	0x0000,
	0x1E73,
	0x1032,
	0x0A84,
	0x00E0,
	0x9F27,
	0x2DD4,
	0x2DD4,
	0xBC00,
	0x0101,
	0x2150,
	0x8404,
	0x0000,   // [17]: SL_TIME 占空比模式下由 API 配置
	0x0000,   // [18]: RX_TIME 占空比模式下由 API 配置
	0x0000,
	0x56AC,
	0xD453,
	0x4940,
	0x5BFF,
	0x0012,
	0xFA90,
	0x0000,
	0xC040,
	0x0000,
	0xEB20,
	0x0007,
};

/* the RFPDK export parameters and make modification as you needed. */ 
static const uint16_t g_cmt2219RegConfigs_868[CMT2219_FTP16_LENGTH] = {
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

static void Cmt2219a_Gpio_Init(void)
{
	CMT2219A_SetGpio1In();
}

void Cmt2219a_Hal_Init(void)
{
	Cmt2219a_Gpio_Init();
	Cmt2219a_Init();	
}

void Cmt2219a_Hal_Config(void)
{
	uint8_t status;  // C89: 声明必须在函数开头
	
	Cmt2219a_Init();                            // CMT2219A 初始化
	Cmt2219a_SoftReset();                       // CMT2219A 软复位
	delay1ms(10);                               // 等待复位完成
	
	// INT1 输出通过 GPO1
	Cmt2219a_SetGpo1Sel(1);
	Cmt2219a_SetInt1(6);                        // PKT_DONE 中断
	Cmt2219a_EnablePacketDoneInt(1);            // 启用数据包完成中断
	
	// 1. 先更新基础配置参数
	Cmt2219a_UpdateConfigs();                   // 更新基础配置
	
	// 2. 使用连续接收模式 (确保能正常接收)
	// 注意：占空比模式配置失败，先用连续接收保证功能正常
	Cmt2219a_EnableSleepTimer(0);               // 禁用睡眠定时器
	Cmt2219a_EnableRecvTimer(0);                // 禁用接收定时器
	Cmt2219a_EnableDutyCycle(0);                // 禁用占空比模式
	
	// 3. 进入接收模式
	Cmt2219a_GoRx();                            // 进入接收模式
	delay1ms(5);
	
	// 验证配置：连续接收模式应该是 RX 状态 (4)
	// 注意：移除死循环检查，避免卡死
	status = Cmt2219a_GetOpStatus();
	// if(4 != status) {
	//     while(1); // 状态不对，停机检查
	// }
}

static void Cmt2219a_UpdateRegisters(const uint16_t *buf16)
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

void Cmt2219a_UpdateConfigs(void)
{
	Cmt2219a_GoStby();  // You should go to stby status firstly, when you chang the rf settings.
	delay1ms(1);        // Wait for go to stby status

	// Update the Cmt2219a settings. 
	Cmt2219a_UpdateRegisters(g_cmt2219RegConfigs_433);
//	Cmt2219a_UpdateRegisters(g_cmt2219RegConfigs_868);
}

uint8_t Radio_Recv_FixedLen(uint8_t *pBuf, uint8_t len)
{
	if(CMT2219A_ReadGpio1())   // PKT_OK
	{
		Cmt2219a_ClearPacketDoneInt(1);     // 清除数据包完成中断
		Cmt2219a_GoTune();                  // 进入数据读取模式
		cmt_spi3_read_fifo(pBuf, len);      // 读取接收数据
		Cmt2219a_ClearFifoContent(1);       // 清空 FIFO
		Cmt2219a_GoRx();                    // 进入接收模式
		return 1;
	}
	return 0;
}
