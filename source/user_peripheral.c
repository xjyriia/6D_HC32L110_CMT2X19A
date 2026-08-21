#include "user_common_config.h"
#include "user_peripheral.h"
#include "gpio.h"
#include "wdt.h"
//#include "lptim.h"
#include "adc.h"
#include "flash.h"


//--------------------------------------------------------------------------------------------
static Irq_Callback_t mIrqCallback = NULL;

//---------------------------------------系统时钟-------------------------------------------
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

//--------------------------------------------------------------------------------------------
/////< WDT 中断服务程序
//void Wdt_IRQHandler(void)
//{
//    if(Wdt_GetIrqStatus())
//    {
//        Wdt_IrqClr();       ///<清除 wdt 中断标记
//				log_printf("%s\r\n", __func__);
//    }
//}

//void user_wdt_init(void)
//{
//    ///< 开启WDT外设时钟
//    Sysctrl_SetPeripheralGate(SysctrlPeripheralWdt,TRUE);
//    ///< WDT 初始化
//    Wdt_Init( WdtResetEn, WdtT3s28);//WdtT6s55);
//    ///< 开启NVIC中断
//    EnableNvic(WDT_IRQn, IrqLevel3, TRUE);
//}

//void user_wdt_start(void)
//{
//    ///< 启动 WDT
//    Wdt_Start();
//}

//void user_wdt_feed(void)
//{
//		Wdt_Feed();
//}
//-----------------------------------------LED指示灯-----------------------------------------


//------------------------------------------低功耗IO配置---------------------------------------
//空脚：
//低功耗IO配置
void user_lowPowerModeGpioSet(void)
{

	
}

