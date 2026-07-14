#ifndef __USER_MOTOR_H__
#define __USER_MOTOR_H__

#include "ddl.h"

/*===========================================================================
 * IO 引脚宏定义
 *===========================================================================*/
#define MOTOR_A_PORT    (2u)    /* 电机 A 相引脚端口 (P24) */
#define MOTOR_A_PIN     (4u)    /* 电机 A 相引脚位号 (P24) */
#define MOTOR_B_PORT    (2u)    /* 电机 B 相引脚端口 (P25) */
#define MOTOR_B_PIN     (5u)    /* 电机 B 相引脚位号 (P25) */

/*===========================================================================
 * 档位定义（传入 Motor_Start 的 gear 参数）
 *===========================================================================*/
#define MOTOR_GEAR_1    (0u)    /* 1档 */
#define MOTOR_GEAR_2    (1u)    /* 2档 */
#define MOTOR_GEAR_3    (2u)    /* 3档 */
#define MOTOR_GEAR_4    (3u)    /* 4档 */

/*===========================================================================
 * 电机运行状态枚举
 *===========================================================================*/
typedef enum
{
    MOTOR_STATE_IDLE    = 0u,   /* 空闲（未启动 / 时序已完成） */
    MOTOR_STATE_FWD     = 1u,   /* A相正转 PWM 进行中   */
    MOTOR_STATE_STOP    = 2u,   /* 正反转间停止缓冲中   */
    MOTOR_STATE_REV     = 3u,   /* B相反转 PWM 进行中   */
    MOTOR_STATE_DONE    = 4u,   /* 全部时序完成         */
} motor_state_t;

/*===========================================================================
 * 对外接口
 *===========================================================================*/

/* 初始化电机 GPIO 及定时器（上电后调用一次） */
void Motor_Init(void);

/* 按指定档位启动电机正反转时序
 *   gear : MOTOR_GEAR_1 ~ MOTOR_GEAR_4
 */
void Motor_Start(uint8_t gear);

/* 立即停止电机，两路引脚拉低 */
void Motor_Stop(void);

/* 获取当前电机运行状态
 * 返回值：MOTOR_STATE_IDLE / FWD / STOP / REV / DONE
 * MOTOR_STATE_DONE 表示正反转时序已全部完成 */
motor_state_t Motor_GetState(void);

/* 电机休眠：停止输出 → 关闭 GPIO/BT 时钟 → 进入 Sleep 低功耗模式
 * 由 Motor_Stop() 自动调用，也可主动调用
 * 唤醒条件：任意已使能中断（如外部 IO 中断） */
void Motor_Sleep(void);

/* 电机唤醒：恢复 GPIO/BT 外设时钟（Sleep 唤醒后调用）
 * 唤醒后若需重新启动，再调用 Motor_Start() */
void Motor_WakeUp(void);

#endif /* __USER_MOTOR_H__ */
