/*
 * bsp_uart.c
 *
 *  Created on: 2026年6月13日
 *      Author: lianz
 */

#include "bsp_uart.h"
#include "usart.h"
#include "stm32f1xx_hal.h"

/* UART配置结构体 */
typedef struct
{
    UART_PortTypeDef port;
    UART_HandleTypeDef* uart_handle;
    void (*tx_callback)(void* context);
    void (*rx_callback)(void* context);
    void* tx_context;
    void* rx_context;
} BSP_UART_ConfigTypeDef;


/* 配置表 */
BSP_UART_ConfigTypeDef uart_config_tables[UART_PORT_MAX] = {
    {UART_PORT_1, 0, 0, 0, 0},
    {UART_PORT_2, &huart2, 0, 0, 0},
    {UART_PORT_3, 0, 0, 0, 0},
    {UART_PORT_4, 0, 0, 0, 0},
    {UART_PORT_5, 0, 0, 0, 0},
    {UART_PORT_6, 0, 0, 0, 0}
};


// 
HAL_StatusTypeDef BSP_UART_StartReceiveIdle(UART_PortTypeDef uart_port,
                                                uint8_t* data,
                                                uint16_t len,
                                                void (*callback)(void* context),
                                                void* context)
{
    BSP_UART_ConfigTypeDef* config = 0;
    /* 检查端口合法性 */
    for (int i = 0; i < UART_PORT_MAX; i++)
    {
        if (uart_config_tables[i].port == uart_port)
        {
            config = &uart_config_tables[i];
            break;
        }
    }
    if (config == 0 || config->uart_handle == 0 || callback == 0)
    {
        return HAL_ERROR;
    }

    /* 配置回调函数 */
    config->rx_callback = callback;
    config->rx_context = context;

    /* 开启UART接收和DMA空闲中断 */
    if (HAL_UARTEx_ReceiveToIdle_DMA(config->uart_handle, data, len) != HAL_OK)
    {
        return HAL_ERROR;
    }

    return HAL_OK;
}



// DMA发送数据
HAL_StatusTypeDef BSP_UART_Transmit_DMA(UART_PortTypeDef uart_port, uint8_t* data, uint16_t len, void (*callback)(void* context), void* context)
{
    BSP_UART_ConfigTypeDef* config = 0;
    /* 检查端口合法性 */
    for (int i = 0; i < UART_PORT_MAX; i++)
    {
        if (uart_config_tables[i].port == uart_port)
        {
            config = &uart_config_tables[i];
            break;
        }
    }
    if (config == 0 || config->uart_handle == 0)
    {
        return HAL_ERROR;
    }

    // 配置回调函数
    config->tx_callback = callback;
    config->tx_context = context;

    /* 发送数据 */
    if (HAL_UART_Transmit_DMA(config->uart_handle, data, len) != HAL_OK)
    {
        return HAL_ERROR;
    }

    return HAL_OK;
}

// DMA开启接收
HAL_StatusTypeDef BSP_UART_Receive_DMA(UART_PortTypeDef uart_port, 
                        uint8_t* rx_buffer, 
                        uint16_t rx_buffer_size, 
                        void (*callback)(void* context),
                        void* context)
{
    BSP_UART_ConfigTypeDef* config = 0;
    /* 检查端口合法性 */
    for (int i = 0; i < UART_PORT_MAX; i++)
    {
        if (uart_config_tables[i].port == uart_port)
        {
            config = &uart_config_tables[i];
            break;
        }
    }
    if (config == 0 || config->uart_handle == 0 || callback == 0 || rx_buffer == 0 || rx_buffer_size == 0)
    {
        return HAL_ERROR;
    }

    /* 配置回调函数 */
    config->rx_callback = callback;
    config->rx_context = context;
    /* 开启UART接收 */
    if (HAL_UART_Receive_DMA(config->uart_handle, rx_buffer, rx_buffer_size) != HAL_OK)
    {
        return HAL_ERROR;
    }
    return HAL_OK;
}

// UART接收完成回调函数
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    BSP_UART_ConfigTypeDef* config = 0;
    /* 检查串口合法性 */
    for (int i = 0; i < UART_PORT_MAX; i++)
    {
        if (uart_config_tables[i].uart_handle == huart)
        {
            config = &uart_config_tables[i];
            break;
        }
    }
    if (config == 0 || config->rx_callback == 0)
    {
        return;
    }

    /* 调用接收回调函数 */
    config->rx_callback(config->rx_context);
}

// UART发送完成回调函数
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    BSP_UART_ConfigTypeDef* config = 0;
    /* 检查串口合法性 */
    for (int i = 0; i < UART_PORT_MAX; i++)
    {
        if (uart_config_tables[i].uart_handle == huart)
        {
            config = &uart_config_tables[i];
            break;
        }
    }
    if (config == 0 || config->tx_callback == 0)
    {
        return;
    }

    /* 调用发送回调函数 */
    config->tx_callback(config->tx_context);
}

// DMA空闲中断回调函数
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    BSP_UART_ConfigTypeDef* config = 0;
    /* 检查串口合法性 */
    for (int i = 0; i < UART_PORT_MAX; i++)
    {
        if (uart_config_tables[i].uart_handle == huart)
        {
            config = &uart_config_tables[i];
            break;
        }
    }
    if (config == 0 || config->rx_callback == 0)
    {
        return;
    }

    /* 调用接收回调函数 */
    config->rx_callback(config->rx_context);
}