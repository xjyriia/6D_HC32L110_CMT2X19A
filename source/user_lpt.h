#ifndef __USER_LPT_H__
#define __USER_LPT_H__

#include <stdint.h>

void LptTimer_Init(void);
void LptTimer_DeInit(void);
uint32_t *lpt_flag_get(void);


#endif


