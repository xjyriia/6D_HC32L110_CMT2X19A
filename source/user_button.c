#include "user_button.h"
#include "board.h"
#include "gpio.h"
#include "ddl.h"

// 消抖配置
#define KEY_DEBOUNCE_MS    20     // 消抖时间 20ms

static volatile uint8_t key_pending = 0;      // 按键待确认标志

/**
 * @brief  设置按键标志 (供 main.c 的 Gpio_IRQHandler 调用)
 */
void key_set_pending(void)
{
    key_pending = 1;
}

/**
 * @brief  检查按键是否有效 (带延时消抖)
 * @retval 1: 按键有效, 0: 无效
 */
uint8_t key_is_pressed(void)
{
#if defined(BOARD_TYPE_TX)
    if (key_pending) {
        // 延时消抖
        delay1ms(KEY_DEBOUNCE_MS);
        
        // 检查按键是否仍然按下 (低电平有效)
        if (Gpio_GetIO(KEY_PORT, KEY_PIN) == 0) {
            return 1;
        } else {
            // 抖动，清除标志
            key_pending = 0;
        }
    }
#endif
    return 0;
}

/**
 * @brief  清除按键标志
 */
void key_clear_flag(void)
{
    key_pending = 0;
}

void key_init(void)
{
    // 初始化按键引脚为输入上拉
    Gpio_InitIOExt(KEY_PORT, KEY_PIN, GpioDirIn, TRUE, FALSE, FALSE, 0);
    
    // 配置 GPIO 中断（下降沿触发）
    Gpio_ClearIrq(KEY_PORT, KEY_PIN);
    Gpio_EnableIrq(KEY_PORT, KEY_PIN, GpioIrqFalling);
    EnableNvic(KEY_IRQ, DDL_IRQ_LEVEL_DEFAULT, TRUE);
}
