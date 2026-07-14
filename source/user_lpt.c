
/******************************************************************************
 * Include files
 ******************************************************************************/
#include "user_lpt.h"
#include "ddl.h"
#include "lpt.h"
#include "gpio.h"
#include "clk.h"

static uint8_t lpt_init_en = FALSE;
/******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
static volatile uint32_t u32Lpt10msFlag = 0;

uint32_t *lpt_flag_get(void)
{
		return (uint32_t *)&u32Lpt10msFlag;
}
/******************************************************************************
 * LPT 10ms定时中断服务函数
 ******************************************************************************/
static void Lpt10msCallback(void)
{
    if (TRUE == Lpt_GetIntFlag())
    {
        Lpt_ClearIntFlag();
        u32Lpt10msFlag++;
        
        // 在这里添加10ms周期性处理代码
        // Gpio_SetIO(1, 5, !Gpio_GetIO(1, 5));  // 调试用，已禁用
        //Lpm_GotoLpmMode();
    }
}

/******************************************************************************
 * LPT 10ms定时器初始化
 * 时钟源: IRC32K (32.768kHz)
 * 定时周期: 10ms
 ******************************************************************************/
void LptTimer_Init(void)
{
    stc_lpt_config_t stcLptConfig;
    uint16_t u16ArrData;
    if(!lpt_init_en){
			// 使能RCL时钟(IRC32K需要)
			Clk_Enable(ClkRCL, TRUE);
			
			// 使能LPT外设时钟
			Clk_SetPeripheralGate(ClkPeripheralLpTim, TRUE);
			
			// LPT配置
			stcLptConfig.enGateP    = LptPositive;      // 门控极性:高电平有效(未使用)
			stcLptConfig.enGate     = LptGateDisable;   // 禁用门控
			stcLptConfig.enTckSel   = LptIRC32K;        // 时钟源: IRC32K (32.768kHz)
			stcLptConfig.enTog      = LptTogDisable;    // 禁用翻转输出
			stcLptConfig.enCT       = LptTimer;         // 定时器模式
			stcLptConfig.enMD       = LptMode2;         // 自动重装载模式
			stcLptConfig.pfnLpTimCb = Lpt10msCallback;  // 中断回调函数
			
			Lpt_Init(&stcLptConfig);
			
			// 计算ARR值: 10ms @ 32.768kHz
			// 计数值 = 32768 * 0.01 = 327.68 ≈ 328
			// ARR = 0xFFFF - 328 + 1 = 0xFEB8
			u16ArrData = 0xFFFF - 328 + 1;  // = 0xFEB8
			
			// 等待ARR可写
			while(!M0P_LPTIMER->CR_f.WT_FLAG);
			Lpt_ARRSet(u16ArrData);
			
			// 清除中断标志并使能中断
			Lpt_ClearIntFlag();
			Lpt_EnableIrq();
			EnableNvic(LPTIM_IRQn, 3, TRUE);
			// 启动定时器
			Lpt_Run();
			
			lpt_init_en = TRUE;
		}
}

/******************************************************************************
 * LPT 反初始化 (休眠时调用)
 * 停止定时器、禁用中断、关闭外设时钟
 ******************************************************************************/
void LptTimer_DeInit(void)
{
		if(lpt_init_en){
			// 停止定时器
			Lpt_Stop();
			
			// 禁用中断
			Lpt_DisableIrq();
			EnableNvic(LPTIM_IRQn, 3, FALSE);
			
			// 清除中断标志
			Lpt_ClearIntFlag();
			
			// 关闭LPT外设时钟
			Clk_SetPeripheralGate(ClkPeripheralLpTim, FALSE);
			
			// 关闭RCL时钟 (如果其他模块不使用)
			Clk_Enable(ClkRCL, FALSE);
			
			lpt_init_en = FALSE;
		}
}
