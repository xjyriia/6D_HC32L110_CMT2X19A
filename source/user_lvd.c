#include "user_lvd.h"
#include "lvd.h"
#include "board.h"

static stc_lvd_config_t stcLvdCfg;

static uint8_t cnt = 0;
static void LvdIrqCallback(void)
{
    //Lvd_DisableIrq();
		cnt++;
}

uint8_t lvd_init(void)
{
    DDL_ZERO_STRUCT(stcLvdCfg);//变量清0

    if (Ok != Clk_SetPeripheralGate(ClkPeripheralGpio, TRUE))//开GPIO时钟
    {
        return 1;
    }
    if (Ok != Clk_SetPeripheralGate(ClkPeripheralVcLvd, TRUE))//开LVD时钟
    {
        return 1;
    }
    //Gpio_SetFunc_LVDOUT_P25();
    // Gpio_SetFunc_LVDOUT_P15();
		
    //配置LVD输入口为模拟端口
    Gpio_SetAnalog(2, 5, TRUE);
    //Gpio_SetAnalog(2, 3, TRUE);
    // Gpio_SetAnalog(0, 3, TRUE);

    stcLvdCfg.bLvdReset = FALSE;//配置中断或者RESET功能
    stcLvdCfg.enInput = LvdInputP25;//配置LVD输入口
    stcLvdCfg.enThreshold = LvdTH2p6V;//配置LVD基准电压
    stcLvdCfg.bFilter = TRUE;
    stcLvdCfg.enFilterTime = LvdFilter29ms;
    stcLvdCfg.enIrqType = LvdIrqRise;
    stcLvdCfg.pfnIrqCbk = LvdIrqCallback;

    Lvd_Init(&stcLvdCfg);
    Lvd_EnableIrq(stcLvdCfg.enIrqType);
    delay1ms(1);
    Lvd_Enable();//LVD使能
}

void lvd_irq_en(uint8_t enable)
{
		if(enable){
				Lvd_EnableIrq(stcLvdCfg.enIrqType);
		}else{
				Lvd_DisableIrq();
		}
}




