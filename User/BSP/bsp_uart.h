/*
 * bsp_uart.h
 *
 *  Created on: 2026年6月13日
 *      Author: lianz
 */

#ifndef BSP_UART_H_
#define BSP_UART_H_

#include <stdint.h>
#include "usart.h"

typedef enum
{
    UART_PORT_1 = 0,
    UART_PORT_2,
    UART_PORT_3,
    UART_PORT_4,
    UART_PORT_5,
    UART_PORT_6,
    UART_PORT_MAX
} UART_PortTypeDef;

/**
 * @brief 启动接收并开启DMA空闲中断回调
 * @param uart_port UART端口枚举
 * @param data 接收缓冲区指针
 * @param len 接收长度
 * @param callback 回调函数指针，参数为回调上下文指针
 * @param context 回调函数上下文指针，用户自定义参数
 */
HAL_StatusTypeDef BSP_UART_StartReceiveIdle(UART_PortTypeDef uart_port,
                                                uint8_t* data,
                                                uint16_t len,
                                                void (*callback)(void* context),
                                                void* context);


/**
 * @brief DMA发送数据
 * @param uart_port UART端口枚举
 * @param data 数据指针
 * @param len 数据长度
    * @param callback 发送完成回调函数指针，参数为回调上下文指针
    * @param context 回调函数上下文指针，用户自定义参数
 */
HAL_StatusTypeDef BSP_UART_Transmit_DMA(UART_PortTypeDef uart_port, 
                        uint8_t* data, 
                        uint16_t len, 
                        void (*callback)(void* context),
                        void* context);

/**
 * @brief DMA开启接收
 * @param uart_port UART端口枚举
 * @param rx_buffer 接收缓冲区指针
 * @param rx_buffer_size 接收缓冲区大小
 * @param callback 接收完成回调函数指针，参数为接收数据和长度
 * @param context 回调函数上下文指针，用户自定义参数，在回调函数中通过该指针访问
 */
HAL_StatusTypeDef BSP_UART_Receive_DMA(UART_PortTypeDef uart_port, 
                        uint8_t* rx_buffer, 
                        uint16_t rx_buffer_size, 
                        void (*callback)(void* context),
                        void* context);

#endif /* BSP_UART_H_ */
