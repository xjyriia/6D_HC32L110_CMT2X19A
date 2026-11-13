#ifndef __USER_COMMON_CONFIG_H__
#define __USER_COMMON_CONFIG_H__

#include "board.h"
#include <stdint.h>

//-----------------------------------------------------------------------
//#define __DEBUG_LOG

#ifdef __DEBUG_LOG
#define	log_printf					printf
#else
//#define	log_printf(...)
#endif

//---------------------------------------------------------------
typedef enum{
	SET_6D 		= 0x01,
	UPLOAD_6D = 0x02,
	PAIR 			= 0X5A,
}cmd_t;

#pragma pack(push, 1)
typedef struct {
	uint8_t len;
	uint8_t cmd;
	uint16_t pair_data;
	uint8_t data;
}rf_data_t;
#pragma pack(pop)

#define PAIR_ADDR			0x00007E00
//-----------------------------------------------------------------------
// 板子类型定义（通过宏切换来区分三块板）
// 在 MDK 选项中定义: --predefine "BOARD_TYPE_6D"
//                   --predefine "BOARD_TYPE_TX" 
//                   --predefine "BOARD_TYPE_RX"
//-----------------------------------------------------------------------
#if !defined(BOARD_TYPE_6D) && !defined(BOARD_TYPE_TX) && !defined(BOARD_TYPE_RX)
//#define BOARD_TYPE_6D  // 默认为 6D 识别板 (DICE)
//#define BOARD_TYPE_RX
#endif

// 根据板子类型启用相应的功能模块
#if defined(BOARD_TYPE_6D)
	#define BOARD_HAS_LIS2DH      // 6D 识别功能
	#define BOARD_HAS_CMT2119A    // 发射功能
//	#define BOARD_HAS_CMT2219A    // 接收功能（200ms 间隔接收模式）
//#define BOARD_NEED_MOTOR      // 电机控制
//	#define BOARD_NEED_LOW_POWER  // 低功耗管理
#elif defined(BOARD_TYPE_TX)
	#define BOARD_HAS_CMT2119A    // 发射功能
	#define BOARD_NEED_LOW_POWER  // 低功耗管理
#elif defined(BOARD_TYPE_RX)
	#define BOARD_HAS_CMT2219A    // 接收功能
	#define BOARD_NEED_VIBRATOR   // 振动马达控制
	#define BOARD_NEED_LOW_POWER  // 低功耗管理
#endif

//-----------------------------------------------------------------------
//
#if defined(LORA_MASTER)
#define	LORA_RX
#elif defined(LORA_SLAVE)
#define	LORA_TX
#endif	//LORA_MASTER


#endif	//__USER_COMMON_CONFIG_H__
