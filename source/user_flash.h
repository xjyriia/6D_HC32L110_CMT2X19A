#ifndef __USER_FLASH_H__
#define __USER_FLASH_H__

#include <stdint.h>
#include <flash.h>

void user_flash_init(void);
en_result_t user_flash_writeByte(uint32_t u32Addr, uint16_t u16Data);
uint16_t user_flash_readByte(uint32_t u32Addr);

#endif


