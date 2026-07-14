#include "user_vibrator.h"
#include "board.h"
#include "gpio.h"
#include "pca.h"
#include "ddl.h"

// 外部 tick 获取函数
extern uint32_t *lpt_flag_get(void);

// 振动马达配置参数
#define VIBRATOR_PWM_FREQ       1000u       // PWM 频率 1kHz
#define VIBRATOR_DUTY_CYCLE     50u         // 占空比 50%

// 振动状态
static uint8_t u8VibratorRunning = 0;
static uint8_t u8LongVibrateTimes = 0;    // 长振次数 (表示5)
static uint8_t u8ShortVibrateTimes = 0;   // 短振次数 (表示1)
static uint8_t u8CurrentLongCount = 0;    // 当前长振计数
static uint8_t u8CurrentShortCount = 0;   // 当前短振计数
static uint32_t u32VibrateStartTick = 0;  // 振动开始 tick
static uint8_t u8VibratePhase = 0;        // 0: 振动中, 1: 间隔中
static uint8_t u8VibrateMode = 0;         // 0: 长振阶段, 1: 短振阶段

// 振动时间配置 (tick 单位, 1 tick = 10ms)
#define LONG_VIBRATE_ON_TICKS    50      // 长振时间 500ms
#define SHORT_VIBRATE_ON_TICKS   15      // 短振时间 150ms
#define VIBRATE_OFF_TICKS        15      // 间隔时间 150ms
#define MODE_GAP_TICKS           30      // 长短振切换间隔 300ms

/**
 * @brief  振动马达 GPIO 初始化
 * @param  None
 * @retval None
 */
static void Vibrator_GpioInit(void)
{
#if defined(BOARD_TYPE_RX)
    // 接收板：使用 MOTOR_CTRL 引脚作为振动马达控制引脚
    Gpio_InitIO(MOTOR_CTRL_PORT, MOTOR_CTRL_PIN, GpioDirOut);
    Gpio_SetIO(MOTOR_CTRL_PORT, MOTOR_CTRL_PIN, 0);
#else
    // 6D 识别板不使用振动马达
    // 如果需要，可以在这里添加 6D 板的替代方案
#endif
}

/**
 * @brief  PCA PWM 初始化 (用于振动马达)
 * @param  None
 * @retval None
 */
static void Vibrator_PCA_Init(void)
{
    stc_pca_config_t stcPcaConfig;
    stc_pca_capmodconfig_t stcCapModConfig;
    
    DDL_ZERO_STRUCT(stcPcaConfig);
    DDL_ZERO_STRUCT(stcCapModConfig);
    
    // PCA 时钟配置：PCLK/3，假设 PCLK=4MHz，则 PCA 时钟=1.33MHz
    stcPcaConfig.enCPS = PCAPCLKDiv3;
    stcPcaConfig.enWDTE = PCAWDTDisable;
    stcPcaConfig.enCIDL = IdleGoon;  // 休眠模式继续工作
    stcPcaConfig.pfnPcaCb = NULL;
    
    Pca_Init(&stcPcaConfig);
    
    // 配置 PCA 模块 0 为 PWM 输出
    stcCapModConfig.enECOM = ECOMEnable;
    stcCapModConfig.enCAPP = CAPPDisable;
    stcCapModConfig.enCAPN = CAPNDisable;
    stcCapModConfig.enMAT = MATEnable;
    stcCapModConfig.enTOG = TOGDisable;
    stcCapModConfig.enPWM = PCAPWMEnable;
    
    Pca_CapModConfig(Module0, &stcCapModConfig);
    
    // 设置 PWM 占空比
    // PCA 计数器溢出值 = 65536
    // PWM 频率 = PCA 时钟 / 65536 = 1.33MHz / 65536 ≈ 20Hz
    // 需要调整 PCA 时钟分频比来获得 1kHz PWM
    Pca_CapData16Set(Module0, 1330);  // 占空比控制
    
    // 使能 PCA 中断
    Pca_EnableCntIrq();
    
    // 启动 PCA
    Pca_Run();
}

/**
 * @brief  振动马达驱动初始化
 * @param  None
 * @retval None
 */
void user_vibrator_drv_init(void)
{
    Vibrator_GpioInit();
    Vibrator_PCA_Init();
    u8VibratorRunning = 0;
    u8LongVibrateTimes = 0;
    u8ShortVibrateTimes = 0;
    u8CurrentLongCount = 0;
    u8CurrentShortCount = 0;
}

/**
 * @brief  设置振动数值
 * @param  num: 数值 (长振=5, 短振=1)
 *         例: 6 = 1个长振 + 1个短振
 *             4 = 4个短振
 *             15 = 3个长振
 * @retval None
 */
void user_vibrator_set(uint8_t num)
{
#if defined(BOARD_TYPE_RX)
    if (num > 0 && !u8VibratorRunning) {
        // 计算长振和短振次数
        u8LongVibrateTimes = num / 5;   // 长振次数 (5的倍数)
        u8ShortVibrateTimes = num % 5;  // 短振次数 (余数)
        u8CurrentLongCount = 0;
        u8CurrentShortCount = 0;
        u8VibratorRunning = 1;
        u8VibratePhase = 0;  // 振动中
        u8VibrateMode = (u8LongVibrateTimes > 0) ? 0 : 1;  // 有长振先长振
        u32VibrateStartTick = *lpt_flag_get();
        
        // 启动振动
        Gpio_SetIO(MOTOR_CTRL_PORT, MOTOR_CTRL_PIN, 1);
    }
#endif
}

/**
 * @brief  停止振动
 * @param  None
 * @retval None
 */
void user_vibrator_stop(void)
{
#if defined(BOARD_TYPE_RX)
    u8VibratorRunning = 0;
    u8VibratePhase = 0;
    Gpio_SetIO(MOTOR_CTRL_PORT, MOTOR_CTRL_PIN, 0);
#endif
}

/**
 * @brief  获取振动状态
 * @param  None
 * @retval 1: 正在振动，0: 未振动
 */
uint8_t user_vibrator_is_running(void)
{
    return u8VibratorRunning;
}

/**
 * @brief  振动 tick 处理 (在主循环中周期调用)
 *         长振=5, 短振=1, 先长振后短振
 * @param  None
 * @retval None
 */
void user_vibrator_tick(void)
{
#if defined(BOARD_TYPE_RX)
    uint32_t u32CurTick;
    uint32_t u32Elapsed;
    uint32_t u32OnTicks;
    
    if (!u8VibratorRunning) {
        return;
    }
    
    u32CurTick = *lpt_flag_get();
    u32Elapsed = u32CurTick - u32VibrateStartTick;
    
    // 根据当前模式选择振动时长
    u32OnTicks = (u8VibrateMode == 0) ? LONG_VIBRATE_ON_TICKS : SHORT_VIBRATE_ON_TICKS;
    
    if (u8VibratePhase == 0) {
        // 振动中, 检查是否到达振动时间
        if (u32Elapsed >= u32OnTicks) {
            // 停止振动
            Gpio_SetIO(MOTOR_CTRL_PORT, MOTOR_CTRL_PIN, 0);
            
            if (u8VibrateMode == 0) {
                // 长振阶段
                u8CurrentLongCount++;
                if (u8CurrentLongCount >= u8LongVibrateTimes) {
                    // 长振完成，切换到短振
                    if (u8ShortVibrateTimes > 0) {
                        u8VibrateMode = 1;
                        u8VibratePhase = 1;  // 进入模式切换间隔
                        u32VibrateStartTick = u32CurTick;
                    } else {
                        // 无短振，振动完成
                        u8VibratorRunning = 0;
                    }
                } else {
                    // 进入长振间隔
                    u8VibratePhase = 1;
                    u32VibrateStartTick = u32CurTick;
                }
            } else {
                // 短振阶段
                u8CurrentShortCount++;
                if (u8CurrentShortCount >= u8ShortVibrateTimes) {
                    // 短振完成，振动结束
                    u8VibratorRunning = 0;
                } else {
                    // 进入短振间隔
                    u8VibratePhase = 1;
                    u32VibrateStartTick = u32CurTick;
                }
            }
        }
    } else {
        // 间隔中
        uint32_t u32GapTicks;
        
        // 长短振切换用较长间隔
        if (u8VibrateMode == 1 && u8CurrentShortCount == 0) {
            u32GapTicks = MODE_GAP_TICKS;
        } else {
            u32GapTicks = VIBRATE_OFF_TICKS;
        }
        
        if (u32Elapsed >= u32GapTicks) {
            // 启动下一次振动
            Gpio_SetIO(MOTOR_CTRL_PORT, MOTOR_CTRL_PIN, 1);
            u8VibratePhase = 0;
            u32VibrateStartTick = u32CurTick;
        }
    }
#endif
}