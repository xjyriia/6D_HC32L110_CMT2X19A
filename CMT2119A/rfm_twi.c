#include "rfm_twi.h"
#include "board.h"
#include "gpio.h"

void twi_init(void)
{
	Gpio_InitIO(TWI_SCK_PORT, TWI_SCK_PIN, GpioDirOut);
	Gpio_InitIO(TWI_SDA_PORT, TWI_SDA_PIN, GpioDirOut);
}

void clk_out(void)
{
	Gpio_InitIO(TWI_SCK_PORT, TWI_SCK_PIN, GpioDirOut);
}

void data_out(void)
{
	Gpio_InitIO(TWI_SDA_PORT, TWI_SDA_PIN, GpioDirOut);
}

void clk_in(void)
{
	Gpio_InitIO(TWI_SCK_PORT, TWI_SCK_PIN, GpioDirIn);
}

void data_in(void)
{
	Gpio_InitIO(TWI_SDA_PORT, TWI_SDA_PIN, GpioDirIn);
}

uint8_t read_data(void)
{
	return Gpio_GetIO(TWI_SDA_PORT, TWI_SDA_PIN);
}

void TWI_WrByte(uint8_t SendByte)
{
  uint8_t i = 0;

  for(i = 0; i < 8; i++)
  {
      rfm_twi_clk_1();
      if(SendByte & 0x80) //MSB
      {
          rfm_twi_data_1();
      }
      else
      {
          rfm_twi_data_0();
      }
      SendByte <<= 1;
      delay10us(1);
      rfm_twi_clk_0();
      delay10us(1);
  }
}

uint8_t TWI_RdByte(void)
{
  uint8_t i = 0;
	uint8_t RdPara = 0;

  for(i = 0; i < 8; i++)
  {
      rfm_twi_clk_1();
		  delay10us(1);
			RdPara <<= 1;
      rfm_twi_clk_0();
			data_in();
      delay10us(1);
      if(rfm_twi_data_read()) //MSB
      {
          RdPara |= 0x01;
      }
      else
      {
          RdPara |= 0x00;
      }		
  }
	return RdPara;
}

void TWI_WRREG(uint8_t WriteAddress, uint8_t SendByte)
{
  WriteAddress |= 0x80;

  //rfm_twi_data_out();
  rfm_twi_data_1();
  rfm_twi_data_out();
  rfm_twi_clk_1();
  rfm_twi_clk_out();

  TWI_WrByte(WriteAddress);

  TWI_WrByte(SendByte);
 
  rfm_twi_data_1();
  rfm_twi_data_od();
 
  rfm_twi_clk_1();
  rfm_twi_clk_od();
}

void TWI_RDREG(uint8_t ReadAddress,uint8_t *pData)
{
  ReadAddress |= 0xC0;

  //rfm_twi_data_out();
  rfm_twi_data_1();
  rfm_twi_data_out();
  rfm_twi_clk_1();
  rfm_twi_clk_out();
  TWI_WrByte(ReadAddress);

	rfm_twi_data_1();
  rfm_twi_data_od();
	
  *pData = TWI_RdByte();
 
  rfm_twi_data_1();
  rfm_twi_data_od();
 
  rfm_twi_clk_1();
  rfm_twi_clk_od();
}

void TWI_RST(void)
{
	rfm_twi_data_1();
	rfm_twi_data_out();
	rfm_twi_clk_1();
	rfm_twi_clk_out();

	// DATA pin low for 32 clock cycles and clocking in 0x8D00
	TWI_WrByte(0x00);
	TWI_WrByte(0x00);
	TWI_WrByte(0x00);
	TWI_WrByte(0x00);

	TWI_WrByte(0x8D);
	TWI_WrByte(0x00);

	rfm_twi_clk_1();
  rfm_twi_clk_od();
	rfm_twi_data_1();
	rfm_twi_data_od();
}

void SOFT_RST(void)
{
	TWI_WRREG(0x3D,0x01);  //0xBD01
}

void TWI_OFF(void)
{
	TWI_WRREG(0x0D,0x02);  //0x8D02
}

void TWI_Init(void)
{
	rfm_twi_clk_1();
	rfm_twi_clk_out();

	rfm_twi_data_1();
	rfm_twi_data_out();	
}

void system_delay_1us(uint32_t n)
{
    while(n--) 
    {
        __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
    }
}

void Transimission(uint8_t pBuf[], uint8_t len)
{
	int i;

	uint8_t j;
	for(i=0; i<len; i++){
		for(j=0x80; j!=0; j>>=1){
			if((pBuf[i]&j)!=0){
					rfm_twi_data_1();
			}else{
					rfm_twi_data_0();
			}
			
			rfm_twi_data_out();
			//delay100us (1);
			//system_delay_1us(47);
			//system_delay_1us(110);		//48M 2.4kbps
			system_delay_1us(47);		//48M 4.8kbps

//		delay1us(417);  //SymbolTime
//		delay1us(425);  //SymbolTime
 		}
	}
}









