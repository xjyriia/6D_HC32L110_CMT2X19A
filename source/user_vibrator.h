#ifndef __USER_VIBRATOR_H__
#define __USER_VIBRATOR_H__

#include<stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void user_vibrator_drv_init(void);
void user_vibrator_set(uint8_t vn);
void user_vibrator_stop(void);
void user_vibrator_tick(void);      // 在主循环中周期调用
uint8_t user_vibrator_is_running(void);

#ifdef __cplusplus
}
#endif

#endif


