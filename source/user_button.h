#ifndef __USER_BUTTON_H__
#define __USER_BUTTON_H__

#include <stdint.h>

void key_init(void);
uint8_t key_is_pressed(void);   // 检查按键是否触发
void key_clear_flag(void);       // 清除按键标志
void key_set_pending(void);
#endif


