#include "lis2dh_drv.h"
#include "gpio.h"
#include "board.h"
#include "sim_i2c.h"
#include <stdbool.h>


// 向LIS2DH12写入数据
static uint8_t LIS2DH12_Write(uint8_t reg, uint8_t data) {
    return iic_Write_Bytes(LIS2DH_ADDR, reg, &data, 1);
}

// 从LIS2DH12读取数据
static uint8_t LIS2DH12_Read(uint8_t reg, uint8_t *data, uint8_t len) {
    return iic_Read_Bytes(LIS2DH_ADDR, reg, data, len);
}




void i2c_write_byte(uint8_t addr, uint8_t reg, uint8_t data)
{
		iic_Write_Bytes(addr, reg, &data, 1);
}

uint8_t i2c_read_byte(uint8_t addr, uint8_t reg)
{
		uint8_t data = 0;
		iic_Read_Bytes(addr, reg, &data, 1);
		return data;
}

void i2c_read_bytes(uint8_t addr, uint8_t reg, uint8_t *data, uint8_t len)
{
		iic_Read_Bytes(addr, reg, data, len);
}

uint8_t LIS2DH_ReadReg(uint8_t ucRegAddr)
{
		uint8_t data = 0;
		iic_Read_Bytes(LIS2DH_ADDR, ucRegAddr, &data, 1);
		return data;
}

void LIS2DH_WriteReg(uint8_t ucRegAddr, uint8_t ucData)
{
		iic_Write_Bytes(LIS2DH_ADDR, ucRegAddr, &ucData, 1);
}

SixDPosition current_position;

// 初始化LIS2DH
// 初始化LIS2DH为低功耗6D识别模式
uint8_t LIS2DH_Init(void)
{
    uint8_t who_am_i;
		uint8_t ctrl3;
		uint8_t int1_cfg;
    uint8_t dummy;
		uint8_t ctrl_reg0 = 0;
    
    // 初始化I2C
    i2c_init();
    delay1ms(10);
    
    // 检查设备ID
    who_am_i = LIS2DH_ReadReg(LIS2DH_REG_WHO_AM_I);
    if (who_am_i != 0x33)
        return 1; // 设备不存在

		// 读取 CTRL_REG0 的当前值，保留其他位的状态，仅将最高位（Bit 7）置 1
		ctrl_reg0 = LIS2DH_ReadReg(LIS2DH_REG_CTRL_REG0); 
		LIS2DH_WriteReg(LIS2DH_REG_CTRL_REG0, ctrl_reg0 | 0x80);			//最高位写1，断开SA0内部上拉电阻
		
		//6D 位置识别
    LIS2DH_WriteReg(LIS2DH_REG_CTRL_REG1, 0x2F);		//10HZ LPen Xen Yen Zen 分别置 1
    LIS2DH_WriteReg(LIS2DH_REG_CTRL_REG2, 0x00);		//
    LIS2DH_WriteReg(LIS2DH_REG_CTRL_REG3, 0x40);
    LIS2DH_WriteReg(LIS2DH_REG_CTRL_REG4, 0x80);
  
    LIS2DH_WriteReg(LIS2DH_REG_CTRL_REG5, 0x00);        // 中断不锁存
    // LIS2DH_WriteReg(LIS2DH_REG_CTRL_REG5, 0x08);        // 中断锁存 (I1_LIR=1)，允许重复触发
    LIS2DH_WriteReg(LIS2DH_REG_INT1_THS, 0x15);					//调节触发阈值
    LIS2DH_WriteReg(LIS2DH_REG_INT1_DURATION, 0x05);	//0x00 6D识别静止500ms算有效
    LIS2DH_WriteReg(LIS2DH_REG_INT1_CFG, 0xFF); //0xff 全方向 6D 检测
		
		LIS2DH_ClearInterrupt();
		return 0;
}

// 从传感器读取6D位置
SixDPosition LIS2DH_Get6DPosition(void)
{
    uint8_t int_src;
    
    // 读取中断源寄存器获取6D状态
    int_src = LIS2DH_ReadReg(LIS2DH_REG_INT1_SRC);
    
    // 根据中断源判断位置
    if (int_src & 0x01) return POSITION_X_UP;
    if (int_src & 0x02) return POSITION_X_DOWN;
    if (int_src & 0x04) return POSITION_Y_UP;
    if (int_src & 0x08) return POSITION_Y_DOWN;
    if (int_src & 0x10) return POSITION_Z_UP;
    if (int_src & 0x20) return POSITION_Z_DOWN;
    
    return POSITION_UNKNOWN;
}

// 清除中断标志
void LIS2DH_ClearInterrupt(void)
{
    // ��ȡINT1_SRC�Ĵ������Զ�����жϱ�־
    uint8_t status = LIS2DH_ReadReg(LIS2DH_REG_INT1_SRC);
    //int_occurred = 0;
}

static uint8_t ucDirStatus;
static uint8_t cnt = 0;

// 6D 数据变量
static uint8_t s_u86DData = 0;                /**< 6D 姿态数据 */
static volatile uint8_t s_u86DReceived = 0;   /**< 6D 数据接收标志 */

/**
 * @brief  LIS2DH 中断处理函数（6D 识别板专用）
 * @param  None
 * @retval None
 */
void LIS2DH_Int_Handler(void)
{
    SixDPosition position;
    
    // 清除中断标志
    Gpio_ClearIrq(G_INT_PORT, G_INT_PIN);
    
    // 获取 6D 位置
    position = LIS2DH_Get6DPosition();
    
    // 保存 6D 数据
    s_u86DData = (uint8_t)position;
    s_u86DReceived = 1;
    
    // 清除 LIS2DH 中断标志
//    LIS2DH_ClearInterrupt();
}

/**
 * @brief  获取 6D 数据
 * @param  None
 * @retval 6D 姿态数据
 */
uint8_t LIS2DH_Get6DData(void)
{
    return s_u86DData;
}

/**
 * @brief  检查是否有新的 6D 数据
 * @param  None
 * @retval 1: 有新数据, 0: 无新数据
 */
uint8_t LIS2DH_Is6DDataReceived(void)
{
    return s_u86DReceived;
}

/**
 * @brief  清除 6D 数据接收标志
 * @param  None
 * @retval None
 */
void LIS2DH_Clear6DDataFlag(void)
{
    s_u86DReceived = 0;
}

void lis2dh_int_exit_init(void)
{
    Gpio_InitIOExt(G_INT_PORT, G_INT_PIN, GpioDirIn, FALSE, FALSE, FALSE, 0);
    
    Gpio_ClearIrq(G_INT_PORT, G_INT_PIN);
    Gpio_EnableIrq(G_INT_PORT, G_INT_PIN, GpioIrqRising);//GpioIrqRising
    EnableNvic(G_INT_IRQ, DDL_IRQ_LEVEL_DEFAULT, TRUE);
}


