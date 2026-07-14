#include "user_flash.h"
#include <flash.h>

static volatile uint32_t u32FlashTestFlag	= 0;


/*******************************************************************************
 * FLASH 中断服务函数
 ******************************************************************************/
static void FlashInt(void)
{
		if (TRUE == Flash_GetIntFlag(flash_int0))
		{
				Flash_ClearIntFlag(flash_int0);
				u32FlashTestFlag |= 0x01;
				Flash_DisableIrq(flash_int0);
		}
		if (TRUE == Flash_GetIntFlag(flash_int1))
		{
				Flash_ClearIntFlag(flash_int1);
				u32FlashTestFlag |= 0x02;
				Flash_DisableIrq(flash_int1);
		} 
}

void user_flash_init(void)
{
		Flash_Init(FlashInt, 0);
}
 
en_result_t user_flash_writeByte(uint32_t u32Addr, uint16_t u16Data)
{
		en_result_t       enResult = Error;
		Flash_SectorErase(u32Addr);

		enResult = Flash_WriteHalfWord(u32Addr, u16Data);
		if (Ok == enResult)
		{
				if(*((volatile uint8_t*)u32Addr) == u16Data)
				{
						enResult = Ok;
				}
				else
				{
						return enResult;
				}
		}
		else
		{
				enResult = Error;
				return enResult;
		}
		return enResult;
}

uint16_t user_flash_readByte(uint32_t u32Addr)
{
		return *((volatile uint16_t*)u32Addr);
}
