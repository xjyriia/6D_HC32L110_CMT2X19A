#ifndef __USER_DRV_CONFIG_H__
#define __USER_DRV_CONFIG_H__

#include <stdint.h>
//--------------------------------------------------------------------------------------------
//¿ª¹Ø
typedef enum{
	OFF,
	ON,
}onoff_t;

//--------------------------------------------------------------------------------------------
typedef enum{
	IRQ_LORA,
	IRQ_S_H,
	IRQ_S_L,
	IRQ_LPTIMER,
	IRQ_ADC,
}irq_cb_t;

//--------------------------------------------------------------------------------------------
typedef void (*Irq_Callback_t)(irq_cb_t cb, void *data);

//--------------------------------------------------------------------------------------------
void user_sysClk_init(void);

void user_wdt_init(void);
void user_wdt_start(void);
void user_wdt_feed(void);

void user_led_init(void);
void user_led_set(onoff_t onoff);

void user_pwrCtrl_init(void);
void user_pwrCtrl_set(onoff_t onoff);

void user_exit_init(Irq_Callback_t callback);
void user_exit_enable(void);

void user_LPTimer_init(Irq_Callback_t callback);
void user_setLPTimerGate(void);
void user_LPTimerStart(void);

void user_adc_init(Irq_Callback_t callback);
void user_adcSglCfg(void);

void user_flash_init(uint16_t *data);
void user_flash_write(uint16_t *data);

void user_lowPowerModeGpioSet(void);

#endif
