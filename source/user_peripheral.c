#include "user_common_config.h"
#include "user_peripheral.h"
#include "gpio.h"
#include "wdt.h"
//#include "lptim.h"
#include "adc.h"
#include "flash.h"


//--------------------------------------------------------------------------------------------
static Irq_Callback_t mIrqCallback = NULL;

//---------------------------------------ϵͳʱ��-------------------------------------------
//ϵͳʱ�ӳ�ʼ��
void user_sysClk_init(void)
{
    stc_clk_config_t stcClkCfg;
    stcClkCfg.enClkSrc  = ClkRCH;
    stcClkCfg.enHClkDiv = ClkDiv1;
    stcClkCfg.enPClkDiv = ClkDiv1;
    
    Clk_Init(&stcClkCfg);
		//ʹ���ڲ�����RC������RCL��
		Clk_Enable(ClkRCL, TRUE);
}

//--------------------------------------------------------------------------------------------
/////< WDT �жϷ������
//void Wdt_IRQHandler(void)
//{
//    if(Wdt_GetIrqStatus())
//    {
//        Wdt_IrqClr();       ///<��� wdt �жϱ��
//				log_printf("%s\r\n", __func__);
//    }
//}

//void user_wdt_init(void)
//{
//    ///< ����WDT����ʱ��
//    Sysctrl_SetPeripheralGate(SysctrlPeripheralWdt,TRUE);
//    ///< WDT ��ʼ��
//    Wdt_Init( WdtResetEn, WdtT3s28);//WdtT6s55);
//    ///< ����NVIC�ж�
//    EnableNvic(WDT_IRQn, IrqLevel3, TRUE);
//}

//void user_wdt_start(void)
//{
//    ///< ���� WDT
//    Wdt_Start();
//}

//void user_wdt_feed(void)
//{
//		Wdt_Feed();
//}
//-----------------------------------------LEDָʾ��-----------------------------------------


//------------------------------------------�͹���IO����---------------------------------------
//�սţ�
//�͹���IO����
void user_lowPowerModeGpioSet(void)
{

	
}

