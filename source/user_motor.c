#include "user_motor.h"
#include "clk.h"
#include "gpio.h"
#include "bt.h"
#include "lpm.h"

/*===========================================================================
 * 单阶段参数结构体
 *   port / pin      — GPIO 端口号 / 引脚位号
 *   high_us         — 高电平宽度  (us)；停止阶段置 0
 *   period_us       — PWM 周期    (us)；停止阶段等于 duration_us
 *   duration_us     — 本阶段总时长 (us)
 *===========================================================================*/
typedef struct
{
    uint8_t  port;
    uint8_t  pin;
    uint32_t high_us;
    uint32_t period_us;
    uint32_t duration_us;
} motor_phase_cfg_t;

/*===========================================================================
 * 4档时序参数表（对照时序图，单位已换算为 us）
 * 每档 3 个阶段：[0] A相正转PWM  [1] 停止缓冲  [2] B相反转PWM
 *
 * 档位  port pin  high_us  period_us  duration_us   频率/占空比
 * ----  ---- ---  -------  ---------  -----------   ----------
 * 1档A  2    4      150       574        144000    1.741kHz 74%
 *    停止  -    -        0    200000        200000    0.2s
 *    B  2    5     1434     51720       1570000    19.3Hz  2.8%  (1.55s+20ms)
 * 2档A  2    4       45       574        144000    1.741kHz 92%
 *    停止  -    -        0    200000        200000    0.2s
 *    B  2    5     1646     51930       1570000    19.3Hz  3.2%
 * 3档A  2    4      186       574        144000    1.741kHz 67%
 *    停止  -    -        0    200000        200000    0.2s
 *    B  2    5     1364     51620       1570000    19.3Hz  2.6%
 * 4档A  2    4       81       574        144000    1.741kHz 86%
 *    停止  -    -        0    200000        200000    0.2s
 *    B  2    5     1575     51840       1570000    19.3Hz  3.0%
 *===========================================================================*/
#define GEAR_PHASE_NUM  (3u)    /* 每档阶段数：A相正转 / 停止 / B相反转 */
#define GEAR_NUM        (4u)    /* 档位总数 */

static const motor_phase_cfg_t s_gear_table[GEAR_NUM][GEAR_PHASE_NUM] =
{
    /* 1档 */  {
        { MOTOR_A_PORT, MOTOR_A_PIN,  150u,    574u,  144000u }, /* A相 1.741kHz 74%  0.144s */
        { MOTOR_A_PORT, MOTOR_A_PIN,    0u, 200000u,  200000u }, /* 停止             0.200s */
        { MOTOR_B_PORT, MOTOR_B_PIN, 1434u,  51720u, 1570000u }, /* B相 19.3Hz  2.8% 1.570s */
    },
    /* 2档 */  {
        { MOTOR_A_PORT, MOTOR_A_PIN,   45u,    574u,  144000u }, /* A相 1.741kHz 92%  0.144s */
        { MOTOR_A_PORT, MOTOR_A_PIN,    0u, 200000u,  200000u }, /* 停止             0.200s */
        { MOTOR_B_PORT, MOTOR_B_PIN, 1646u,  51930u, 1570000u }, /* B相 19.3Hz  3.2% 1.570s */
    },
    /* 3档 */  {
        { MOTOR_A_PORT, MOTOR_A_PIN,  186u,    574u,  144000u }, /* A相 1.741kHz 67%  0.144s */
        { MOTOR_A_PORT, MOTOR_A_PIN,    0u, 200000u,  200000u }, /* 停止             0.200s */
        { MOTOR_B_PORT, MOTOR_B_PIN, 1364u,  51620u, 1570000u }, /* B相 19.3Hz  2.6% 1.570s */
    },
    /* 4档 */  {
        { MOTOR_A_PORT, MOTOR_A_PIN,   81u,    574u,  144000u }, /* A相 1.741kHz 86%  0.144s */
        { MOTOR_A_PORT, MOTOR_A_PIN,    0u, 200000u,  200000u }, /* 停止             0.200s */
        { MOTOR_B_PORT, MOTOR_B_PIN, 1575u,  51840u, 1570000u }, /* B相 19.3Hz  3.0% 1.570s */
    },
};

typedef enum { PH_HIGH, PH_LOW, PH_IDLE } pwm_phase_t;

static volatile uint8_t      s_cur_gear     = 0u;            /* 当前运行档位 (0~3)   */
static volatile uint8_t      s_phase_idx    = 0u;            /* 当前阶段索引 0/1/2   */
static volatile pwm_phase_t  s_phase        = PH_HIGH;
static volatile uint32_t     s_section_time = 0u;            /* 当前阶段已运行时间(us)*/
static volatile motor_state_t s_motor_state = MOTOR_STATE_IDLE; /* 对外运行状态        */

/* 内部函数声明 */
static void Motor_GpioInit(void);
static void Motor_TimerInit(void);
static void Motor_SetTimer(uint32_t us);
static void Motor_TimerCallback(void);

/*---------------------------------------------------------------------------
 * Motor_GpioInit — 初始化 A相、B相引脚为输出低
 *---------------------------------------------------------------------------*/
static void Motor_GpioInit(void)
{
    Clk_SetPeripheralGate(ClkPeripheralGpio, TRUE);
    Gpio_InitIO(MOTOR_A_PORT, MOTOR_A_PIN, GpioDirOut);
    Gpio_InitIO(MOTOR_B_PORT, MOTOR_B_PIN, GpioDirOut);
    Gpio_SetIO(MOTOR_A_PORT, MOTOR_A_PIN, FALSE);
    Gpio_SetIO(MOTOR_B_PORT, MOTOR_B_PIN, FALSE);
}

/*---------------------------------------------------------------------------
 * Motor_TimerInit — 初始化 TIM0，注册中断回调
 *---------------------------------------------------------------------------*/
static void Motor_TimerInit(void)
{
    stc_bt_config_t stcBtCfg;

    Clk_SetPeripheralGate(ClkPeripheralBt, TRUE);
    DDL_ZERO_STRUCT(stcBtCfg);
    stcBtCfg.enGateP   = BtPositive;
    stcBtCfg.enGate    = BtGateDisable;
    stcBtCfg.enPRS     = BtPCLKDiv4;
    stcBtCfg.enTog     = BtTogDisable;
    stcBtCfg.enCT      = BtTimer;
    stcBtCfg.enMD      = BtMode2;
    stcBtCfg.pfnTim0Cb = Motor_TimerCallback;
    stcBtCfg.pfnTim1Cb = NULL;
    stcBtCfg.pfnTim2Cb = NULL;

    Bt_Init(TIM0, &stcBtCfg);
    Bt_ClearIntFlag(TIM0);
    Bt_EnableIrq(TIM0);
    EnableNvic(TIM0_IRQn, 3u, TRUE);
}

/*---------------------------------------------------------------------------
 * Motor_SetTimer — 装载定时器计数值（内部辅助）
 *   us : 期望计时时长，超出 0xFFFF 则截位到 0xFFFF
 *---------------------------------------------------------------------------*/
static void Motor_SetTimer(uint32_t us)
{
    if (us > 0xFFFFu) us = 0xFFFFu;
    Bt_ARRSet(TIM0,   (uint16_t)(0xFFFF - us));
    Bt_Cnt16Set(TIM0, (uint16_t)(0xFFFF - us));
}

/*---------------------------------------------------------------------------
 * Motor_Init — 初始化电机 GPIO 及定时器（上电后调用一次）
 *---------------------------------------------------------------------------*/
void Motor_Init(void)
{
    Motor_GpioInit();
    Motor_TimerInit();
}

/*---------------------------------------------------------------------------
 * Motor_Start — 按指定档位启动电机正反转时序
 *   gear : MOTOR_GEAR_1 ~ MOTOR_GEAR_4 (0~3)
 *---------------------------------------------------------------------------*/
void Motor_Start(uint8_t gear)
{
    const motor_phase_cfg_t *pFirst;

    if (gear >= GEAR_NUM) gear = 0u;

    s_cur_gear     = gear;
    s_phase_idx    = 0u;
    s_phase        = PH_HIGH;
    s_section_time = 0u;
    s_motor_state  = MOTOR_STATE_FWD;   /* 启动即进入正转阶段 */

    pFirst = &s_gear_table[s_cur_gear][0];
    Gpio_SetIO(pFirst->port, pFirst->pin, TRUE);
    Motor_SetTimer(pFirst->high_us);
    Bt_Run(TIM0);
}

/*---------------------------------------------------------------------------
 * Motor_Stop — 停止定时器，两路引脚拉低，然后进入休眠
 *---------------------------------------------------------------------------*/
void Motor_Stop(void)
{
    Bt_Stop(TIM0);
    Gpio_SetIO(MOTOR_A_PORT, MOTOR_A_PIN, FALSE);
    Gpio_SetIO(MOTOR_B_PORT, MOTOR_B_PIN, FALSE);
    s_motor_state = MOTOR_STATE_DONE;   /* 标记时序完成 */
    Motor_Sleep();   /* 停止后自动进入休眠 */
}

/*---------------------------------------------------------------------------
 * Motor_Sleep — 关闭 GPIO/BT 外设时钟，进入 Sleep 低功耗模式
 *   唤醒条件：任意已使能中断（如外部 IO 中断）
 *---------------------------------------------------------------------------*/
void Motor_Sleep(void)
{
    stc_lpm_config_t stcLpmCfg;

    /* 关闭外设时钟，降低待机功耗 */
//    Clk_SetPeripheralGate(ClkPeripheralBt,   FALSE);
//    Clk_SetPeripheralGate(ClkPeripheralGpio, FALSE);

    /* 配置为普通 Sleep 模式（中断唤醒） */
    stcLpmCfg.enSEVONPEND   = SevPndDisable;
    stcLpmCfg.enSLEEPDEEP   = SlpDpDisable;   /* Sleep，非 DeepSleep */
    stcLpmCfg.enSLEEPONEXIT = SlpExtDisable;
    Lpm_Config(&stcLpmCfg);

//    Lpm_GotoLpmMode();   /* WFI 进入休眠，中断唤醒后继续执行 */
}

/*---------------------------------------------------------------------------
 * Motor_WakeUp — 唤醒后恢复 GPIO/BT 外设时钟
 *   Sleep 唤醒后若需重新启动电机，调用此函数再调用 Motor_Start()
 *---------------------------------------------------------------------------*/
void Motor_WakeUp(void)
{
    Clk_SetPeripheralGate(ClkPeripheralGpio, TRUE);
    Clk_SetPeripheralGate(ClkPeripheralBt,   TRUE);
}

/*---------------------------------------------------------------------------
 * Motor_GetState — 获取当前电机运行状态
 *---------------------------------------------------------------------------*/
motor_state_t Motor_GetState(void)
{
    return s_motor_state;
}

/*---------------------------------------------------------------------------
 * Motor_TimerCallback — TIM0 中断回调，驱动时序状态机
 *---------------------------------------------------------------------------*/
static void Motor_TimerCallback(void)
{
    const motor_phase_cfg_t *p;
    uint32_t next_us;

    if (Bt_GetIntFlag(TIM0) == FALSE)
    {
        return;
    }
    Bt_ClearIntFlag(TIM0);

    /* 全部阶段完成 */
    if (s_phase_idx >= GEAR_PHASE_NUM)
    {
        Motor_Stop();
        return;
    }

    p = &s_gear_table[s_cur_gear][s_phase_idx];

    /*-- 停止阶段（high_us == 0）：等满 duration_us 后切换下一阶段 --*/
    if (p->high_us == 0u)
    {
        s_section_time += p->duration_us;
        if (s_section_time >= p->duration_us)
        {
            s_phase_idx++;
            s_section_time = 0u;
            s_phase        = PH_HIGH;
            if (s_phase_idx < GEAR_PHASE_NUM)
            {
                const motor_phase_cfg_t *pNext =
                    &s_gear_table[s_cur_gear][s_phase_idx];
                s_motor_state = MOTOR_STATE_REV;   /* 停止缓冲结束，进入反转 */
                Gpio_SetIO(pNext->port, pNext->pin, TRUE);
                Motor_SetTimer(pNext->high_us);
            }
            else
            {
                Motor_Stop();
            }
        }
        return;
    }

    /*-- PWM 阶段：交替翻转引脚电平 --*/
    if (s_phase == PH_HIGH)
    {
        s_section_time += p->high_us;
        s_phase = PH_LOW;
        Gpio_SetIO(p->port, p->pin, FALSE);
        next_us = p->period_us - p->high_us;        /* 低电平时长 */
    }
    else
    {
        s_section_time += (p->period_us - p->high_us);
        s_phase = PH_HIGH;
        Gpio_SetIO(p->port, p->pin, TRUE);
        next_us = p->high_us;                       /* 高电平时长 */
    }

    /* 本阶段时长已满 → 两路引脚全部拉低，推进下一阶段 */
    if (s_section_time >= p->duration_us)
    {
        Gpio_SetIO(MOTOR_A_PORT, MOTOR_A_PIN, FALSE);
        Gpio_SetIO(MOTOR_B_PORT, MOTOR_B_PIN, FALSE);
        s_phase_idx++;
        s_section_time = 0u;
        s_phase        = PH_IDLE;
        s_motor_state  = MOTOR_STATE_STOP;   /* 正转完成，进入停止缓冲 */
        if (s_phase_idx >= GEAR_PHASE_NUM)
        {
            Motor_Stop();
            return;
        }
        /* 装载下一阶段（停止缓冲）时长 */
        next_us = s_gear_table[s_cur_gear][s_phase_idx].duration_us;
    }

    Motor_SetTimer(next_us);
}
