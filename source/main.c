/**
 ******************************************************************************
 * @file    main.c
 * @author  Application Team
 * @version V1.0.0
 * @date    2026-03-14
 * @brief   统一的主程序框架 - 支持三板协同工作
 *          通过宏定义区分：BOARD_TYPE_6D, BOARD_TYPE_TX, BOARD_TYPE_RX
 ******************************************************************************
 */
#include "user_system.h"
#include "user_lpt.h"
#include "ddl.h"
#include "stdbool.h"
#include "board.h"
#include "interrupts_hc32l110.h"
#include "lpt.h"  // LPT 运行函数
#include "bt.h"
#include "user_flash.h"

#define RF_PACKET_SIZE      5      	// 有效数据长度 (不含前导码和同步字)

// CMT2119A 发送数据包格式: 前导码 + 同步字 + 有效数据
// CMT2219A 同步字配置: 0x2DD4 0x2DD4 -> 字节序: 0xD4 0x2D 0xD4 0x2D
#define RF_PREAMBLE_SIZE    4       // 前导码长度
#define RF_SYNC_SIZE        4       // 同步字长度
#define RF_TOTAL_SIZE       (RF_PREAMBLE_SIZE + RF_SYNC_SIZE + RF_PACKET_SIZE)		//1为数据长度参数占用1个字节

//#include "uart.h"

#include "user_button.h"
// 包含板子相关的头文件
#ifdef BOARD_HAS_LIS2DH
#include "lis2dh_drv.h"
#endif

#ifdef BOARD_HAS_CMT2119A
#include "rfm_twi.h"
#include "cmt2119a.h"
#include "cmt2119a_params.h"
#endif

#ifdef BOARD_HAS_CMT2219A
#include "cmt2219a.h"
#include "cmt2219a_hal.h"
#endif

#ifdef BOARD_NEED_MOTOR
#include "user_motor.h"
#endif

#ifdef BOARD_NEED_VIBRATOR
#include "user_vibrator.h"
#endif

#ifdef BOARD_NEED_LOW_POWER
#include "user_low_power.h"
#endif

// 接收数据缓冲区
#define g_recevlength       0x05
unsigned char g_recevingBuf[g_recevlength];

// 6D 板状态机和变量前向声明 (供中断处理使用)
#if defined(BOARD_TYPE_6D)
typedef enum {
    STATE_6D_IDLE = 0,          // 空闲，等待 6D 事件
    STATE_6D_SENDING,           // 正在发送 (重发中)
    STATE_6D_RECV_WAIT,         // 发送完成，等待 TX 响应 (6 秒接收窗口)
    STATE_6D_MOTOR_RUNNING,     // 电机运行中
    STATE_6D_SLEEP              // 进入深度睡眠
} State_6D_t;
extern State_6D_t s_e6DState;   // 外部声明，供中断处理访问
static uint8_t s_u8Cmt2219Init = 0;    // 6D 板 CMT2219A 初始化标志
#endif

// 函数声明
static void Board_Init(void);
static void Board_MainLoop(void);
static void Timeout_Callback(void);

// 外部函数声明
extern void i2c_init(void);
extern void key_init(void);
extern void LptTimer_Init(void);
extern void PcaWdt_Init(void);
extern void user_flash_init(void);
extern void pcaWdt_Feed(void);

uint8_t initCnt = 0;

/**
 * @brief  GPIO 中断处理 (统一入口，各板型根据中断源区分)
 */
void Gpio_IRQHandler(uint8_t u8Param)
{
    // 清除对应中断标志
    *((uint32_t *)((uint32_t)&M0P_GPIO->P0ICLR + u8Param * 0x40)) = 0;
    
#if defined(BOARD_TYPE_RX)
    // RX 板：处理 P3.2 CMT_GPO1 中断 (参数=34)
        // CMT2219A PKT_DONE 中断，唤醒 MCU
        // 不需要额外操作，MCU 已被唤醒
#elif defined(BOARD_TYPE_TX)
    // TX 板：只处理 P0.3 按键中断 (参数=3)
        // 记录按键按下事件
        key_set_pending();
#elif defined(BOARD_TYPE_6D)
    // 6D 板：处理 P0.3 LIS2DH 中断 (仿真时 u8Param=0，实际硬件会检查具体引脚)
    // 直接调用 LIS2DH_Int_Handler()，让 LIS2DH 驱动内部检查中断标志
    LIS2DH_Int_Handler();
    
    // 关键：唤醒后重新启动 LPT（用于振动 tick 和状态机超时）
    LptTimer_DeInit();  // 先重置LPT状态（清除lpt_init_en标志），确保Init能重新执行
    LptTimer_Init();    // 重新初始化LPT（含时钟使能、中断使能）
    Lpt_Run();
    
    // 重置 6D 状态机到 IDLE，允许再次发送
//		if(s_e6DState == STATE_6D_SLEEP)
				s_e6DState = STATE_6D_IDLE;
#endif
}

uint16_t sn_data = 0;
/**
 * @brief  板级初始化
 */
static void Board_Init(void)
{
    System_InitPeriph();
    
    delay1ms(1000);  // 等待系统稳定
    
    // 初始化 I2C 和 LIS2DH (6D 识别板)
#ifdef BOARD_HAS_LIS2DH
    i2c_init();
    lis2dh_int_exit_init();
    LIS2DH_Init();
#endif

    // 初始化按键和低功耗定时器
#if defined(BOARD_TYPE_TX)
    key_init();
    // TX 板不需要 LPT, 只用 GPIO 中断唤醒深度睡眠
#else
    LptTimer_Init();
#endif
    
    // 看门狗初始化 (所有板型)
    PcaWdt_Init();
    
    // Flash 初始化 (所有板型)
    user_flash_init();
		sn_data = user_flash_readByte(PAIR_ADDR);
    
    // CMT2119A 初始化 (发射功能)
#if defined(BOARD_TYPE_TX) || defined(BOARD_TYPE_6D)
#ifdef BOARD_HAS_CMT2119A
		twi_init();
		while(initCnt < 3){
				if (CMT2119A_Init(g_cmt2119aCfg, 21)) {
						break;
				}else{
						initCnt++;
						delay1ms(10);
        }
    }
		CMT2119A_Sleep();
#endif
#endif

    // CMT2219A 初始化 (接收功能)
#if defined(BOARD_TYPE_RX)// || defined(BOARD_TYPE_6D)
#ifdef BOARD_HAS_CMT2219A
    Cmt2219a_Hal_Init();
    Cmt2219a_Hal_Config();
    
    // 注意：占空比模式下不依赖 CMT_GPO1 中断，只用 LPT 定时唤醒轮询
    // 所以这里不配置 GPIO 中断
#endif
#endif

    // 电机驱动初始化
#if defined(BOARD_TYPE_6D)
#ifdef BOARD_NEED_MOTOR
    Motor_Init();                   /* 初始化电机引脚及定时器 */
		Motor_Stop();
#endif
#endif

    // 振动器初始化
#if defined(BOARD_TYPE_RX)
#ifdef BOARD_NEED_VIBRATOR
    user_vibrator_drv_init();
		user_vibrator_set(1);
    // 注意：不在这里启动振动，只在收到数据时振动
#endif
#endif

    // 低功耗管理初始化
#if defined(BOARD_TYPE_RX)
#ifdef BOARD_NEED_LOW_POWER
    user_low_power_init();
#endif
#endif
}

/**
 * @brief  6D 识别板主循环逻辑 (带重发机制，tick 非阻塞)
 */
#if defined(BOARD_TYPE_6D)

// 6D 板状态机和变量 (已在文件开头声明)
static State_6D_t s_e6DState = STATE_6D_SLEEP;//STATE_6D_IDLE;
static uint8_t s_u86DTxCount = 0;           // 已发送次数
static uint32_t s_u326DLastTick = 0;        // 上次发送时的 tick
static uint8_t s_u8Last6D = 0xFF;           // 上次的 6D 数据
static uint32_t s_u32RecvStartTick = 0;     // 开始接收的 tick
static uint8_t s_u8MotorStarted = 0;        // 本次接收窗口内电机是否已启动

// 6D 板发送数据缓冲区 (前导码 + 同步字 + 数据)
static uint8_t s_6DTxData[RF_TOTAL_SIZE]={
    0xAA, 0xAA, 0xAA, 0xAA,               	// 前导码(4B)
		0x2D, 0xD4, 0x2D, 0xD4,      				 		//同步码
    0x04,											    					// 数据包长度
    0x00, 0x00, 0x00, 0x00,     						// data: 命令(1B),配对码(2B),数据(1B)
};

static rf_data_t rf_data = {0};

// 6 秒超时时间 (10ms tick)
#define RECV_WINDOW_TICKS   (6000 / 10)  // 6 秒 = 600 ticks

// tick 间隔计算 (10ms 一个 tick)
#define TX_6D_INTERVAL_TICKS   ((TX_RETRANSMIT_INTERVAL_MS + 9) / 10)

static void Board_6D_Loop(void)
{
    uint32_t u32CurTick;
    uint8_t u8CurData;
    
    u32CurTick = *lpt_flag_get();
    
    switch (s_e6DState) {
    case STATE_6D_IDLE:
        // 空闲状态：检测 6D 变化
        if (LIS2DH_Is6DDataReceived()) {
            LIS2DH_Clear6DDataFlag();
            u8CurData = LIS2DH_Get6DData();
            // 如果 6D 数据发生变化
            if (u8CurData){// != s_u8Last6D) {
                s_u8Last6D = u8CurData;
                
#ifdef BOARD_HAS_CMT2119A
                // Restore CMT2119A pins from analog mode after deep sleep
//                Gpio_SetAnalog(1u, 4u, FALSE);   // TWI_SDA P14
//                Gpio_SetAnalog(1u, 5u, FALSE);   // TWI_SCK P15
//                Gpio_SetAnalog(3u, 3u, FALSE);   // SPI_FCSB P33
//                Gpio_SetAnalog(3u, 4u, FALSE);   // SPI_SCLK P34
//                Gpio_SetAnalog(3u, 6u, FALSE);   // SPI_CSB  P36
//                twi_init();
                {
                    uint8_t initCnt = 0, ret = false;
										while(initCnt < 3){
												twi_init();
                        if (CMT2119A_Init(g_cmt2119aCfg, 21)){
														ret = true;
														break;
												}else{
														delay1ms(10);
														initCnt++;
												}
                    }
                    if (ret) {
                        rf_data.len = (sizeof(rf_data_t)-1);
                        rf_data.cmd = UPLOAD_6D;
                        rf_data.pair_data = user_flash_readByte(PAIR_ADDR);
                        rf_data.data = u8CurData;
                        memcpy(&s_6DTxData[8], &rf_data, sizeof(rf_data_t));
                        CMT2119A_Transmit(s_6DTxData, RF_TOTAL_SIZE);
                        s_u86DTxCount = 1;
                        s_u326DLastTick = u32CurTick;
                        s_e6DState = STATE_6D_SENDING;
                    } else {
                        CMT2119A_Sleep();
                        s_e6DState = STATE_6D_SLEEP;
                    }
                }
#endif
            }
        }else{
            s_e6DState = STATE_6D_SLEEP;
        }
        break;
        
    case STATE_6D_SENDING:
        // 发送状态：检查是否需要重发
#ifdef BOARD_HAS_CMT2119A
        if (s_u86DTxCount >= TX_RETRANSMIT_COUNT) {
            // 重发完成，立即关闭 CMT2119A 进入低功耗
            CMT2119A_Sleep();  // 关断 LDO 和振荡器 (<1μA)
            
#ifdef BOARD_HAS_CMT2219A
            // 重新配置 CMT2219A 进入接收模式 (每次都重新初始化)
						if(!s_u8Cmt2219Init){
								Cmt2219a_Hal_Init();
								Cmt2219a_Hal_Config();
								s_u8Cmt2219Init = 1;
						}
            s_u32RecvStartTick = u32CurTick;
            s_u8MotorStarted = 0;  // 重置电机启动标志
            s_e6DState = STATE_6D_RECV_WAIT;
#else
            // 没有 CMT2219A，直接进入睡眠
            s_e6DState = STATE_6D_SLEEP;
#endif
        } else if ((u32CurTick - s_u326DLastTick) >= TX_6D_INTERVAL_TICKS) {
            // 达到重发间隔，发送下一次
            CMT2119A_Transmit(s_6DTxData, RF_TOTAL_SIZE);
            s_u86DTxCount++;
            s_u326DLastTick = u32CurTick;
        }
#else
        s_e6DState = STATE_6D_SLEEP;
#endif
        break;
        
    case STATE_6D_RECV_WAIT:
        // 等待 TX 响应状态：保持 CMT2219A 接收 6 秒
#ifdef BOARD_HAS_CMT2219A
        // 检查是否收到 TX 板数据
        {
            //static 
						uint8_t rxBuf[RF_PACKET_SIZE] = {0};
            if (Radio_Recv_FixedLen(rxBuf, RF_PACKET_SIZE)) {
								rf_data_t * rf_data = (rf_data_t *)rxBuf;
								if(rf_data->len == 0x10){
										if((rf_data->cmd == SET_6D) && (rf_data->pair_data == user_flash_readByte(PAIR_ADDR)) && (!s_u8MotorStarted)){
												s_u8MotorStarted = 1;  // 标记电机已启动，防止重复启动
												// 收到有效数据，关闭 CMT2219A
												if(s_u8Cmt2219Init){
													Cmt2219a_GoSleep();
													s_u8Cmt2219Init = false;
												}
												
#ifdef BOARD_NEED_MOTOR
												// 启动电机，进入电机运行状态
												Motor_WakeUp();             /* 恢复外设时钟                 */
												Motor_Start(MOTOR_GEAR_2);  /* 重新启动（如需）              */
												s_e6DState = STATE_6D_MOTOR_RUNNING;
#else
												// 没有电机，直接进入睡眠
												s_e6DState = STATE_6D_SLEEP;
#endif
										}else if(rf_data->cmd == PAIR){
												user_flash_writeByte(PAIR_ADDR, rf_data->pair_data);
										}
								}
            }
        }
        
        // 检查是否超过 6 秒 (没收到数据)
        if ((u32CurTick - s_u32RecvStartTick) >= RECV_WINDOW_TICKS) {
            // 6秒超时，没收到数据，关闭射频，进入睡眠
						if(s_u8Cmt2219Init){
								Cmt2219a_GoSleep();
								s_u8Cmt2219Init = false;
						}
            s_e6DState = STATE_6D_SLEEP;
        }
#endif
        break;
        
    case STATE_6D_MOTOR_RUNNING:
        // 电机运行状态：等待电机转完后立即进入睡眠
#ifdef BOARD_NEED_MOTOR
				if (Motor_GetState() == MOTOR_STATE_DONE)
				{
            // 电机转完，立即进入睡眠
            s_e6DState = STATE_6D_SLEEP;
						Motor_Sleep();
				}
#else
        s_e6DState = STATE_6D_SLEEP;
#endif
        break;
        
    case STATE_6D_SLEEP:
        // 睡眠状态：进入深度睡眠
#ifdef BOARD_NEED_LOW_POWER
				pcaWdt_Feed();
        user_enter_deep_sleep_mode();
#endif
        break;
    }
    
    // 喂狗
    pcaWdt_Feed();
}
#endif

/**
 * @brief  发射板主循环逻辑 - 按键唤醒后发送数据 (深度睡眠, GPIO中断唤醒)
 */
#if defined(BOARD_TYPE_TX)
extern uint8_t key_is_pressed(void);
extern void key_clear_flag(void);

// 发送数据缓冲区 (前导码+同步字+数据)
static uint8_t s_TxData[RF_TOTAL_SIZE]={
		0xAA, 0xAA, 0xAA, 0xAA,									//前导码
		0x2D, 0xD4, 0x2D, 0xD4,      						//同步码
		0x10, 																	//数据包长度
		0X00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static rf_data_t rf_data = {0};

static void Board_TX_Loop(void)
{
    uint8_t i;
    
    // 检测按键 (带消抖)
    if (key_is_pressed()) {
        key_clear_flag();
        pcaWdt_Feed();
        
#ifdef BOARD_HAS_CMT2119A
        // 唤醒 CMT2119A
//        CMT2119A_Wakeup();
				twi_init();
				if (FALSE == CMT2119A_Init(g_cmt2119aCfg, 21)) {
        uint8_t i;
        for (i = 0; i < 5; i++) {
            if (CMT2119A_Init(g_cmt2119aCfg, 21)) break;
            delay1ms(10);
        }
    }
				//填充数据
				rf_data.len = 0X10;
				if(1){
					rf_data.cmd = SET_6D;
				}else{
					rf_data.cmd = PAIR;
				}
				rf_data.pair_data = user_flash_readByte(PAIR_ADDR);
				rf_data.data = 0;
				memcpy( &s_TxData[8], &rf_data, sizeof(rf_data_t));
        // 发送数据 (含重发)
        for (i = 0; i <= TX_RETRANSMIT_COUNT; i++) {
            CMT2119A_Transmit(s_TxData, RF_TOTAL_SIZE);
            pcaWdt_Feed();
            if (i < TX_RETRANSMIT_COUNT) {
                delay1ms(TX_RETRANSMIT_INTERVAL_MS);
            }
        }
        
        // 发送完成, CMT2119A 休眠
        CMT2119A_Sleep();
#endif
    }
    
    // 喂狗后进入深度睡眠
    pcaWdt_Feed();
    
#ifdef BOARD_NEED_LOW_POWER
    user_enter_deep_sleep_mode();
#endif
}
#endif

/**
 * @brief  接收板主循环逻辑 - 持续接收，收到数据后振动一次
 */
#if defined(BOARD_TYPE_RX)
#ifdef BOARD_HAS_CMT2219A
extern uint8_t Radio_Recv_FixedLen(uint8_t *pBuf, uint8_t len);
#endif

static void Board_RX_Loop(void)
{
    // 防重复接收变量 (防止重发机制导致的重复振动)
    static uint32_t s_u32LastRecvTick = 0;
    static uint8_t s_u8Last6DData = 0xFF;
    uint32_t u32CurTick;
    
    // 检查是否收到数据 (轮询 CMT2219A 的 GPIO1/PKT_OK 引脚)
#ifdef BOARD_HAS_CMT2219A
    if (Radio_Recv_FixedLen(g_recevingBuf, g_recevlength)) {
        // 收到数据，验证数据包格式
				rf_data_t *rf_data = (rf_data_t *)g_recevingBuf;
				if(rf_data->len == 0x04){
						//6D数据
						if((rf_data->cmd == UPLOAD_6D) && (rf_data->pair_data == user_flash_readByte(PAIR_ADDR))){
								uint8_t u86DData = rf_data->data;  // 获取 6D 数据
								u32CurTick = *lpt_flag_get();
#ifdef BOARD_NEED_VIBRATOR
								// 根据 6D 数据振动对应次数
								// 规则：长振表示 5，短振表示 1
								// 例如：6 = 1 长振 + 1 短振，4 = 4 个短振
								if (u86DData > 0 && u86DData <= 15) {  // 限制有效范围
										// 防重复：相同数据在 500ms 内只处理一次
										if (u86DData != s_u8Last6DData || 
												(u32CurTick - s_u32LastRecvTick) >= 50) {  // 50 ticks = 500ms
												user_vibrator_set(u86DData);
												s_u8Last6DData = u86DData;
												s_u32LastRecvTick = u32CurTick;
										}
								}
#endif
						}else if(rf_data->cmd == PAIR){
								user_flash_writeByte(PAIR_ADDR, rf_data->pair_data);		//保存配对码
						}
				}
    }
#endif
    
    // 振动 tick 处理 (控制振动时长和停止)
#ifdef BOARD_NEED_VIBRATOR
    user_vibrator_tick();
#endif
    
    // 喂狗
    pcaWdt_Feed();
    
    // 短暂延时，确保振动状态更新
#ifdef BOARD_NEED_VIBRATOR
    if (user_vibrator_is_running()) {
        delay1ms(5);  // 振动中时不立即睡眠
        // 振动期间保持正常运行
    } else {
        // 振动完成，进入轻度睡眠 (LPT 80ms 唤醒轮询)
#ifdef BOARD_NEED_LOW_POWER
//        user_enter_sleep_mode();
#endif
    }
#else
    // 无振动功能，直接轻度睡眠
#ifdef BOARD_NEED_LOW_POWER
    user_enter_sleep_mode();
#endif
#endif
}
#endif

/**
 * @brief  板级主循环
 */
static void Board_MainLoop(void)
{
#if defined(BOARD_TYPE_6D)
    Board_6D_Loop();
#elif defined(BOARD_TYPE_TX)
    Board_TX_Loop();
#elif defined(BOARD_TYPE_RX)
    Board_RX_Loop();
#endif
}

/**
 * @brief  主函数
 */
int32_t main(void)
{
    static uint8_t s_u8FirstRun;  // RX 板首次运行计数器（静态变量）
    
    // 板级初始化
    Board_Init();
    
    // 主循环
    while (1) {
        Board_MainLoop();
        
        // RX 板首次上电时先不睡眠，运行几次主循环确保能正常接收
#if defined(BOARD_TYPE_RX)
        if (s_u8FirstRun < 3) {
            s_u8FirstRun++;
            continue;  // 前 3 次不睡眠，确保初始化正常
        }
#endif
    }
}

