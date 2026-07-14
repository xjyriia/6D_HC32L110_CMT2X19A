#include "user_system.h"
#include "user_lpt.h"
#include "user_flash.h"
#include "interrupts_hc32l110.h"

//系统时钟初始化
void user_sysClk_init(void)
{
    stc_clk_config_t stcClkCfg;
    stcClkCfg.enClkSrc  = ClkRCH;
    stcClkCfg.enHClkDiv = ClkDiv1;
    stcClkCfg.enPClkDiv = ClkDiv1;
    
    Clk_Init(&stcClkCfg);
		//使能内部低速RC振荡器（RCL）
		Clk_Enable(ClkRCL, TRUE);
}


static void System_EnablePeriph(void)
{
	
}

static void System_DisablePeriph(void)
{
	
}

void System_InitPeriph(void)
{
		user_sysClk_init();
}


void System_run(void)
{
	
}
