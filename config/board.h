#ifndef __BOARD_H__
#define __BOARD_H__

#include "user_common_config.h"

//---------------------------------------------------------------
#if defined(BOARD_TYPE_6D)
#define	TWI_SCK_PORT					1
#define	TWI_SCK_PIN						5

#define	TWI_SDA_PORT					1
#define	TWI_SDA_PIN						4
#elif defined(BOARD_TYPE_TX)
#define	TWI_SCK_PORT					1
#define	TWI_SCK_PIN						5

#define	TWI_SDA_PORT					1
#define	TWI_SDA_PIN						4
#elif defined(BOARD_TYPE_RX)
// 接收板不使用 TWI，但为了编译通过定义默认值
#define	TWI_SCK_PORT					0xFF
#define	TWI_SCK_PIN						0xFF

#define	TWI_SDA_PORT					0xFF
#define	TWI_SDA_PIN						0xFF
#endif
//---------------------------------------------------------------
#define	CMT_SPI_PORT					3
#define	CMT_SPI_CSB_PIN				    6
#define	CMT_SPI_FCSB_PIN			    3
#define	CMT_SPI_SCLK_PIN			    4
#define	CMT_SPI_SDIO_PIN			    5

#define	CMT_GPO1_PORT					3
#define	CMT_GPO1_PIN					2
#define	CMT_GPO1_IRQ					PORT3_IRQn

#define	CMT_GPO2_PORT					0xFF
#define	CMT_GPO2_PIN					0xFF

#define	CMT_GPO3_PORT					0xFF
#define	CMT_GPO3_PIN					0xFF

//---------------------------------------------------------------
#define	G_SCL_PORT						3
#define	G_SCL_PIN							4

#define	G_SDA_PORT						3
#define	G_SDA_PIN							3

#define	G_INT_PORT						3
#define	G_INT_PIN							5

#define	G_INT_IRQ						PORT3_IRQn

//---------------------------------------------------------------
// 注意：MOTOR_CTRL 引脚根据板子类型不同而不同
// 6D识别板：P2.4 + P2.5 (H 桥双引脚 PWM 控制空心杯电机)
// 接收板：P2.3 (单引脚振动马达)
#if defined(BOARD_TYPE_6D)
#define	MOTOR_A_PORT				    2
#define	MOTOR_A_PIN				      4
#define	MOTOR_B_PORT				    2
#define	MOTOR_B_PIN				      5
#elif defined(BOARD_TYPE_RX)
#define	MOTOR_CTRL_PORT					2
#define	MOTOR_CTRL_PIN					3
#endif

//---------------------------------------------------------------
#define	KEY_PORT						0
#define	KEY_PIN						  3

#define	KEY_IRQ							PORT0_IRQn

//---------------------------------------------------------------
// 重发机制配置 (提高发送成功率)
// 重发次数越多越可靠，但耗时越长
// 建议值: 3-5次，间隔 20-50ms
// 适用于: TX板、 6D板
//---------------------------------------------------------------
#if defined(BOARD_TYPE_TX) || defined(BOARD_TYPE_6D)
#define TX_RETRANSMIT_COUNT         1//5      // 重发次数
#define TX_RETRANSMIT_INTERVAL_MS   30     // 重发间隔(ms)
#endif

//---------------------------------------------------------------
// RX板接收间隔配置 (节省功耗)
// 占空比接收模式: 睡眠时间 + 接收时间
// 睡眠时间越长功耗越低，但响应速度越慢
// 建议值: 50ms-500ms，默认 100ms
//---------------------------------------------------------------
#if defined(BOARD_TYPE_RX)
#define RX_SLEEP_INTERVAL_MS        100    // 接收间隔(ms), 可调: 50/100/200/500
#endif


#endif

