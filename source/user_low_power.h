#ifndef __USER_LOW_POWER_H__
#define __USER_LOW_POWER_H__

#include "stdint.h"
#include "ddl.h"

#ifdef __cplusplus
extern "C" {
#endif

// 函数声明
void user_low_power_init(void);
void user_low_power_start_timer(uint32_t timeout_ms, func_ptr_t callback);
void user_low_power_stop_timer(void);
void user_enter_sleep_mode(void);
void user_enter_deep_sleep_mode(void);
void user_system_enter_low_power_mode(void);
uint8_t user_low_power_is_timer_running(void);

#ifdef __cplusplus
}
#endif

#endif // __USER_LOW_POWER_H__
