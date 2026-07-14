#include "user_common_config.h"
#include "gpio.h"
#include "uart.h"


//串口引脚配置
static void App_UartPortInit(void)
{
    stc_gpio_cfg_t stcGpioCfg;
    
    DDL_ZERO_STRUCT(stcGpioCfg);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE); //GPIO外设模块时钟使能
    
    stcGpioCfg.enDir = GpioDirOut;
    Gpio_Init(GpioPortA,GpioPin9,&stcGpioCfg);
    Gpio_SetAfMode(GpioPortA,GpioPin9,GpioAf1); //配置PA09 为UART0 TX
    stcGpioCfg.enDir = GpioDirIn;
    Gpio_Init(GpioPortA,GpioPin10,&stcGpioCfg);
    Gpio_SetAfMode(GpioPortA,GpioPin10,GpioAf1);//配置PA10 为UART0 RX
}

//串口模块配置
static void App_UartCfg(void)
{
    stc_uart_cfg_t  stcCfg;
    stc_uart_multimode_t stcMulti;
    stc_uart_baud_t stcBaud;

    DDL_ZERO_STRUCT(stcCfg);
    DDL_ZERO_STRUCT(stcMulti);
    DDL_ZERO_STRUCT(stcBaud);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralUart0,TRUE);//UART0外设模块时钟使能
    
    stcCfg.enRunMode = UartMskMode1;     //模式3
    stcCfg.enStopBit = UartMsk1bit;      //1位停止位
    stcCfg.enMmdorCk = UartMskEven;      //偶校验
    stcCfg.stcBaud.u32Baud = 9600;       //波特率9600
    stcCfg.stcBaud.enClkDiv = UartMsk8Or16Div;         //通道采样分频配置
    stcCfg.stcBaud.u32Pclk = Sysctrl_GetPClkFreq();    //获得外设时钟（PCLK）频率值
    Uart_Init(M0P_UART0, &stcCfg);       //串口初始化

    Uart_ClrStatus(M0P_UART0,UartRC);    //清接收请求
    Uart_ClrStatus(M0P_UART0,UartTC);    //清发送请求
    Uart_EnableIrq(M0P_UART0,UartRxIrq); //使能串口接收中断
    Uart_EnableIrq(M0P_UART0,UartTxIrq); //使能串口发送中断
}

void uart_log_init(void)
{
//#ifdef	__DEBUG_LOG
	App_UartPortInit();
	App_UartCfg();
//#endif	
}
