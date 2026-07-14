#ifndef __RFM_TWI_H
#define __RFM_TWI_H

#include <stdbool.h>
#include "gpio.h"
#include "board.h"


//clk--pb7  data--pb3---ppÍÆÍì£¬od¿ªÂ©

#define rfm_twi_clk_od()      clk_out()		//Gpio_InitIOExt(TWI_SCK_PORT, TWI_SCK_PIN, GpioDirIn, false, false, true, 0)	//clk_out()
#define rfm_twi_clk_out()     clk_out()
#define rfm_twi_data_out()    data_out()
#define rfm_twi_data_od()     data_out()		//Gpio_InitIOExt(TWI_SDA_PORT, TWI_SDA_PIN, GpioDirIn, false, false, true, 0)	//data_in()


#define rfm_twi_clk_1()    	  Gpio_SetIO(TWI_SCK_PORT, TWI_SCK_PIN, 1)
#define rfm_twi_clk_0()       Gpio_SetIO(TWI_SCK_PORT, TWI_SCK_PIN, 0)

#define rfm_twi_data_1()      Gpio_SetIO(TWI_SDA_PORT, TWI_SDA_PIN, 1)
#define rfm_twi_data_0()      Gpio_SetIO(TWI_SDA_PORT, TWI_SDA_PIN, 0)

#define rfm_twi_data_read()   read_data()


void twi_init(void);
void clk_out(void);
void data_out(void);
void clk_in(void);
void data_in(void);
uint8_t read_data(void);



void TWI_WrByte(unsigned char SendByte);
uint8_t TWI_RdByte(void);
void TWI_WRREG(uint8_t WriteAddress, uint8_t SendByte);
void TWI_RDREG(uint8_t ReadAddress,uint8_t *pData);
void TWI_RST(void);
void SOFT_RST(void);
void TWI_OFF(void);
void TWI_Init(void);
void Transimission(uint8_t pBuf[], uint8_t len);

#endif
