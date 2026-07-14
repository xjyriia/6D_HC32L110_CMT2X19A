#ifndef __SIM_I2C_H__
#define __SIM_I2C_H__


#include <stdint.h>


void i2c_init(void);
void i2c_uninit(void);

uint8_t iic_Read_Bytes(uint8_t iic_addr,uint8_t read_reg,uint8_t * buff,uint8_t len);
uint8_t iic_Write_Bytes(uint8_t iic_addr,uint8_t write_reg,uint8_t * buff,uint8_t len);
uint8_t iic_Read_Bytes_no_Reg(uint8_t iic_addr,uint8_t * buff,uint8_t len);
uint8_t iic_Write_Bytes_no_Reg(uint8_t iic_addr,uint8_t * buff,uint8_t len);


#endif


