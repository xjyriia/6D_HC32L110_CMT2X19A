#include "sim_i2c.h"
#include <stdint.h>
#include "board.h"
#include "gpio.h"
#include "ddl.h"

// 引脚配置宏定义
#define I2C_SCL_DIR_OUT()					Gpio_InitIOExt(G_SCL_PORT, G_SCL_PIN, GpioDirOut, 1, 0, 0, 1)
#define I2C_SCL_OUT_H()						Gpio_SetIO(G_SCL_PORT, G_SCL_PIN, 1)
#define I2C_SCL_OUT_L()						Gpio_SetIO(G_SCL_PORT, G_SCL_PIN, 0)

#define I2C_SDA_DIR_OUT()					Gpio_InitIOExt(G_SDA_PORT, G_SDA_PIN, GpioDirOut, 1, 0, 0, 1)
#define I2C_SDA_DIR_IN()					Gpio_InitIOExt(G_SDA_PORT, G_SDA_PIN, GpioDirIn, 1, 0, 0, 1)
#define I2C_SDA_OUT_H()						Gpio_SetIO(G_SDA_PORT, G_SDA_PIN, 1)
#define I2C_SDA_OUT_L()						Gpio_SetIO(G_SDA_PORT, G_SDA_PIN, 0)
#define I2C_SDA_IN_VAL()					Gpio_GetIO(G_SDA_PORT, G_SDA_PIN)


// 延时参数（单位：微秒），可根据实际频率调整
#define I2C_DELAY_STD       5       // 标准延时
#define I2C_DELAY_START     6       // 起始信号延时
#define I2C_DELAY_STOP      6       // 停止信号延时
#define I2C_DELAY_ACK       5       // 应答信号延时

// 精确微秒延时函数
__attribute__((section("ram_code"))) 
void i2c_delay(uint16_t us)
{
    // 处理剩余的微秒延时
    volatile uint32_t i;
    // 使用更精确的延时实现，确保延时时间准确
    // 假设delay10us(1)大约延时10us
    if (us >= 10)
    {
        delay10us(us / 10);
        us %= 10;
    }
    
    for (i = 0; i < us * 10; i++)
    {
        __NOP();
    }
}

// 产生IIC起始信号
__attribute__((section("ram_code"))) 
void IIC_Start(void)
{
    I2C_SCL_DIR_OUT();     // SCL线输出
    I2C_SDA_DIR_OUT();     // SDA线输出
    I2C_SDA_OUT_H();	  
    I2C_SCL_OUT_H();
    i2c_delay(I2C_DELAY_START);
    
    I2C_SDA_OUT_L();       // START: 当CLK为高时，DATA从高到低变化
    i2c_delay(I2C_DELAY_START);
    
    I2C_SCL_OUT_L();       // 钳住I2C总线，准备发送或接收数据 
    i2c_delay(I2C_DELAY_STD);
}	  

// 产生IIC停止信号
__attribute__((section("ram_code"))) 
void IIC_Stop(void)
{
    I2C_SDA_DIR_OUT();     // SDA线输出
    I2C_SCL_OUT_L();
    i2c_delay(I2C_DELAY_STD);
    
    I2C_SDA_OUT_L();       // STOP: 当CLK为高时，DATA从低到高变化
    i2c_delay(I2C_DELAY_STD);
    
    I2C_SCL_OUT_H(); 
    i2c_delay(I2C_DELAY_STOP);
    
    I2C_SDA_OUT_H();       // 发送I2C总线结束信号
    i2c_delay(I2C_DELAY_STOP);						   	
}

// 等待应答信号到来
// 返回值：1，接收应答失败；0，接收应答成功
__attribute__((section("ram_code"))) 
uint8_t IIC_Wait_Ack(void)
{
    uint8_t ucErrTime = 0;
    
    I2C_SDA_DIR_IN();      // SDA设置为输入 
    i2c_delay(I2C_DELAY_STD); 
    
    I2C_SCL_OUT_H();
    i2c_delay(I2C_DELAY_ACK);
    
    // 等待应答
    while(I2C_SDA_IN_VAL())
    {
        ucErrTime++;
        if(ucErrTime > 200)  // 增加超时计数，避免无限等待
        {
            IIC_Stop();
            return 1;  // 超时，应答失败
        }
        i2c_delay(1);  // 短暂延时再检查
    }
    
    I2C_SCL_OUT_L();       // 时钟输出0 	   
    i2c_delay(I2C_DELAY_STD);
    return 0;  
}

// 产生ACK应答
__attribute__((section("ram_code"))) 
void IIC_Ack(void)
{
    I2C_SCL_OUT_L();
    I2C_SDA_DIR_OUT();
    I2C_SDA_OUT_L();       // 拉低SDA表示应答
    i2c_delay(I2C_DELAY_ACK);
    
    I2C_SCL_OUT_H();
    i2c_delay(I2C_DELAY_ACK);
    
    I2C_SCL_OUT_L();
    i2c_delay(I2C_DELAY_STD);
}

// 不产生ACK应答		    
__attribute__((section("ram_code"))) 
void IIC_NAck(void)
{
    I2C_SCL_OUT_L();
    I2C_SDA_DIR_OUT();
    I2C_SDA_OUT_H();       // 拉高SDA表示非应答
    i2c_delay(I2C_DELAY_ACK);
    
    I2C_SCL_OUT_H();
    i2c_delay(I2C_DELAY_ACK);
    
    I2C_SCL_OUT_L();
    i2c_delay(I2C_DELAY_STD);
}

// IIC发送一个字节
__attribute__((section("ram_code"))) 
void IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;  
    I2C_SDA_DIR_OUT();     // 确保SDA为输出模式
    I2C_SCL_OUT_L();       // 拉低时钟开始数据传输 
    i2c_delay(I2C_DELAY_STD);
    
    // 发送8位数据，高位在前
    for(t = 0; t < 8; t++)
    {              
        if (txd & 0x80)
            I2C_SDA_OUT_H();
        else
            I2C_SDA_OUT_L();
        
        txd <<= 1; 	  
        i2c_delay(I2C_DELAY_STD);
        
        I2C_SCL_OUT_H();    // 时钟高电平，从机读取数据
        i2c_delay(I2C_DELAY_STD); 
        
        I2C_SCL_OUT_L();    // 拉低时钟，准备下一位数据
        i2c_delay(I2C_DELAY_STD);
    }	 
} 	    

// 读1个字节，ack=1时，发送ACK；ack=0，发送nACK   
__attribute__((section("ram_code"))) 
uint8_t IIC_Read_Byte(unsigned char ack)
{
    unsigned char i, receive = 0;
    I2C_SDA_DIR_IN();      // SDA设置为输入
    
    for(i = 0; i < 8; i++)
    {
        I2C_SCL_OUT_L(); 
        i2c_delay(I2C_DELAY_STD);
        
        I2C_SCL_OUT_H();    // 时钟高电平，读取数据
        receive <<= 1;
        if(I2C_SDA_IN_VAL())
            receive++;   
        i2c_delay(I2C_DELAY_STD); 
    }
    
    I2C_SCL_OUT_L(); 
    i2c_delay(I2C_DELAY_STD);
    
    // 发送应答信号
    if (!ack)
        IIC_NAck();        // 发送nACK
    else
        IIC_Ack();         // 发送ACK   
    
    return receive;
}

// 从指定寄存器读取多个字节
uint8_t iic_Read_Bytes(uint8_t iic_addr, uint8_t read_reg, uint8_t *buff, uint8_t len)
{
    uint8_t ret = 0;
    uint8_t i = 0;
    uint8_t write_addr = (iic_addr << 1);
    uint8_t read_addr = (iic_addr << 1) | 0x01;
    
    if (buff == NULL || len == 0)
        return 1;  // 参数错误
    IIC_Start();
    
    // 发送设备写地址
    IIC_Send_Byte(write_addr);
    ret = IIC_Wait_Ack();
    if (ret)
    {
        IIC_Stop();
        return ret;
    }
    
    // 发送要读取的寄存器地址
    IIC_Send_Byte(read_reg);
    ret = IIC_Wait_Ack();
    if (ret)
    {
        IIC_Stop();
        return ret;
    }
    
    // 重复起始信号，切换到读模式
    IIC_Start();
    IIC_Send_Byte(read_addr);
    ret = IIC_Wait_Ack();
    if (ret)
    {
        IIC_Stop();
        return ret;
    }
    
    // 读取数据
    for(i = 0; i < len; i++)
    {
        // 最后一个字节发送非应答
        buff[i] = IIC_Read_Byte((i != len - 1) ? 1 : 0);
    }
    
    IIC_Stop();
    return ret;
}

// 向指定寄存器写入多个字节
uint8_t iic_Write_Bytes(uint8_t iic_addr, uint8_t write_reg, uint8_t *buff, uint8_t len)
{
    uint8_t ret = 0;
    uint8_t i = 0;
		uint8_t write_addr;
    if (buff == NULL || len == 0)
        return 1;  // 参数错误
    
		write_addr = (iic_addr << 1);

    IIC_Start();
    
    // 发送设备写地址
    IIC_Send_Byte(write_addr);
    ret = IIC_Wait_Ack();
    if (ret)
    {
        IIC_Stop();
        return ret;
    }
    
    // 发送要写入的寄存器地址
    IIC_Send_Byte(write_reg);
    ret = IIC_Wait_Ack();
    if (ret)
    {
        IIC_Stop();
        return ret;
    }
    
    // 写入数据
    for(i = 0; i < len; i++)
    {
        IIC_Send_Byte(buff[i]);
        ret = IIC_Wait_Ack();
        if (ret)
        {
            IIC_Stop();
            return ret;
        }
    }
    
    IIC_Stop();
    return ret;
}

// 初始化I2C总线
void i2c_init(void)
{
    // 初始化SCL引脚为高电平输出
    I2C_SCL_DIR_OUT();
    I2C_SCL_OUT_H();

    // 初始化SDA引脚为高电平输出
    I2C_SDA_DIR_OUT();
    I2C_SDA_OUT_H();
    
    // 初始化后等待一段时间，确保总线稳定
    i2c_delay(100);
}

// 反初始化I2C总线
void i2c_uninit(void)
{
    // 释放引脚，可根据需要配置为输入状态
    I2C_SCL_DIR_OUT();
    I2C_SCL_OUT_H();
    
    I2C_SDA_DIR_OUT();
    I2C_SDA_OUT_H();
}
