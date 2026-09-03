/**
 * @file  fsd13_rs485.h
 * @brief  FSD13-RS485 激光位移传感器驱动头文件
 * @note   基于STM32 HAL库，RS485半双工通信
 */

#ifndef __FSD13_RS485_H
#define __FSD13_RS485_H

#include "stm32f1xx_hal.h"  // 根据你的芯片型号修改

/* ===================== 用户配置区 ===================== */

// 传感器型号选择（用于测量值换算）
typedef enum {
    FSD13_30  = 0,   // 中心距离30mm, 分辨率1μm
    FSD13_50  = 1,   // 中心距离50mm, 分辨率10μm
    FSD13_100 = 2,   // 中心距离100mm, 分辨率10μm
    FSD13_200 = 3,   // 中心距离200mm, 分辨率10μm
    FSD13_400 = 4,   // 中心距离400mm, 分辨率100μm
} FSD13_Model_t;

// 通信协议选择
typedef enum {
    PROTOCOL_MODBUS = 0,   // Modbus RTU协议
    PROTOCOL_CUSTOM = 1,   // 自定义协议
} FSD13_Protocol_t;

// RS485方向控制引脚（半双工需要）
#define RS485_DE_PORT       GPIOA
#define RS485_DE_PIN        GPIO_PIN_3

// UART句柄（根据你的配置修改）
#define FSD13_UART          huart3
extern UART_HandleTypeDef FSD13_UART;

// 默认从机地址（Modbus RTU）
#define FSD13_SLAVE_ADDR    0x01

// 自定义协议站点号（默认0x02，见说明书示例）
#define FSD13_CUSTOM_STX    0x02

/* ===================== 函数声明 ===================== */

void FSD13_Init(FSD13_Model_t model, FSD13_Protocol_t protocol);
uint8_t FSD13_ReadDistance(float *distance_mm);
uint8_t FSD13_LaserOn(void);
uint8_t FSD13_LaserOff(void);
uint8_t FSD13_ZeroSet(void);
uint8_t FSD13_ZeroCancel(void);
uint8_t FSD13_InitSensor(void);  // 传感器初始化（恢复出厂）

#endif