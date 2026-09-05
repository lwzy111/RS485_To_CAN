#ifndef __RS485_H
#define __RS485_H

#include "stm32f1xx_hal.h"
#include "usart.h"
#include "bsp_uart.h"

typedef enum
{
    RS485_PORT_2 = 0,
    RS485_PORT_MAX
} RS485_PortTypeDef;

/**
 * @brief 注册RS485回调
 * @param 接收回调函数
 * @param 发送回调函数
 */
HAL_StatusTypeDef RS485_RegisterCallback(RS485_PortTypeDef rs485_port,
                                         void (*tx_callback)(void* context),
                                         void (*rx_callback)(void* context));

HAL_StatusTypeDef RS485_Transmit_DMA(RS485_PortTypeDef rs485_port,
                                     uint8_t *data,
                                     uint16_t size);
/**
 * @brief DMA开启接收
 * @param rs485_port RS485端口枚举
 * @param data 接收缓冲区指针
 * @param size 接收缓冲区大小
 * @param callback 接收完成回调函数指针，参数为回调上下文指针
 * @param context 回调函数上下文指针，用户自定义参数
 */
HAL_StatusTypeDef RS485_Receive_DMA(RS485_PortTypeDef rs485_port,
                                    uint8_t *data,
                                    uint16_t size);

#endif
