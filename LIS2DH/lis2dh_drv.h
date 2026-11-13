#ifndef __LIS2DH_H
#define __LIS2DH_H

#include <stdint.h>

// LIS2DH I2C��ַ (SA0�ӵ�)
#define LIS2DH_ADDR             0x18

// �Ĵ�����ַ����
#define LIS2DH_REG_STATUS_REG_AUX   0x07
#define LIS2DH_REG_OUT_TEMP_L       0x0C
#define LIS2DH_REG_OUT_TEMP_H       0x0D
#define LIS2DH_REG_WHO_AM_I         0x0F
#define LIS2DH_REG_CTRL_REG0        0x1E
#define LIS2DH_REG_CTRL_REG1        0x20
#define LIS2DH_REG_CTRL_REG2        0x21
#define LIS2DH_REG_CTRL_REG3        0x22
#define LIS2DH_REG_CTRL_REG4        0x23
#define LIS2DH_REG_CTRL_REG5        0x24
#define LIS2DH_REG_CTRL_REG6        0x25
#define LIS2DH_REG_REFERENCE        0x26
#define LIS2DH_REG_STATUS_REG2      0x27
#define LIS2DH_REG_OUT_X_L          0x28
#define LIS2DH_REG_OUT_X_H          0x29
#define LIS2DH_REG_OUT_Y_L          0x2A
#define LIS2DH_REG_OUT_Y_H          0x2B
#define LIS2DH_REG_OUT_Z_L          0x2C
#define LIS2DH_REG_OUT_Z_H          0x2D
#define LIS2DH_REG_FIFO_CTRL_REG    0x2E
#define LIS2DH_REG_FIFO_SRC_REG     0x2F
#define LIS2DH_REG_INT1_CFG         0x30
#define LIS2DH_REG_INT1_SRC         0x31
#define LIS2DH_REG_INT1_THS         0x32
#define LIS2DH_REG_INT1_DURATION    0x33
#define LIS2DH_REG_INT2_CFG         0x34
#define LIS2DH_REG_INT2_SRC         0x35
#define LIS2DH_REG_INT2_THS         0x36
#define LIS2DH_REG_INT2_DURATION    0x37
#define LIS2DH_REG_CLICK_CFG        0x38
#define LIS2DH_REG_CLICK_SRC        0x39
#define LIS2DH_REG_CLICK_THS        0x3A
#define LIS2DH_REG_TIME_LIMIT       0x3B
#define LIS2DH_REG_TIME_LATENCY     0x3C
#define LIS2DH_REG_TIME_WINDOW      0x3D
#define LIS2DH_REG_ACT_THS          0x3E
#define LIS2DH_REG_ACT_DUR          0x3F

// 6Dλ��ö��
//typedef enum {
//    POSITION_UNKNOWN,
//    POSITION_X_UP,
//    POSITION_X_DOWN,
//    POSITION_Y_UP,
//    POSITION_Y_DOWN,
//    POSITION_Z_UP,
//    POSITION_Z_DOWN
//} SixDPosition;

typedef enum {
    POSITION_UNKNOWN,
    POSITION_Z_DOWN,
    POSITION_X_DOWN,
    POSITION_Y_DOWN,
    POSITION_Y_UP,
    POSITION_X_UP,
    POSITION_Z_UP
} SixDPosition;

void lis2dh_int_exit_init(void);
// ��ʼ������
uint8_t LIS2DH_Init(void);

// ��ȡ6Dλ��
SixDPosition LIS2DH_Get6DPosition(void);

// ����жϱ�־
void LIS2DH_ClearInterrupt(void);

// �жϷ�����ԭ��
void LIS2DH_INT_IRQHandler(void);

// ��ȡ��ǰ�ж�״̬
uint8_t LIS2DH_GetInterruptStatus(void);

// LIS2DH 中断处理函数
void LIS2DH_Int_Handler(void);

// 获取 6D 数据
uint8_t LIS2DH_Get6DData(void);

// 检查是否有新的 6D 数据
uint8_t LIS2DH_Is6DDataReceived(void);

// 清除 6D 数据接收标志
void LIS2DH_Clear6DDataFlag(void);

#endif
