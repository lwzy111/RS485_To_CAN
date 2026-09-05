#include "bsp_rs485.h"
#include "gpio.h"
#include "bsp_uart.h"

/* RS485配置结构体 */
typedef struct
{
    RS485_PortTypeDef port;
    UART_PortTypeDef uart_port;
    GPIO_TypeDef* de_port;
    uint16_t de_pin;
    void (*tx_callback)(void* context);
    void (*rx_callback)(void* context);
    void* tx_context;
    void* rx_context;
} BSP_RS485_ConfigTypeDef;

/* 配置表 */
static BSP_RS485_ConfigTypeDef rs485_config_tables[RS485_PORT_MAX] = {
    {RS485_PORT_2, UART_PORT_2, GPIOB, GPIO_PIN_0, 0, 0}
};

static BSP_RS485_ConfigTypeDef* RS485_GetConfig(RS485_PortTypeDef rs485_port);
static void RS485_SetDirection(BSP_RS485_ConfigTypeDef* config, uint8_t direction);
static void RS485_ReceiveCompleteCallback(void* context);
static void RS485_TransmitCompleteCallback(void* context);

HAL_StatusTypeDef RS485_RegisterCallback(RS485_PortTypeDef rs485_port,
                                         void (*tx_callback)(void* context),
                                         void (*rx_callback)(void* context))
{
    if (rs485_port >= RS485_PORT_MAX) return HAL_ERROR;
    if (tx_callback == 0 || rx_callback == 0) return HAL_ERROR;
    BSP_RS485_ConfigTypeDef* config = RS485_GetConfig(rs485_port);
    if (config == 0) return HAL_ERROR;

    // 填充回调函数
    config->tx_callback = tx_callback;
    config->rx_callback = rx_callback;
    config->tx_context = 0;
    config->rx_context = 0;

    return HAL_OK;
}

HAL_StatusTypeDef RS485_Transmit_DMA(RS485_PortTypeDef rs485_port,
                                     uint8_t *data,
                                     uint16_t size)
{
    BSP_RS485_ConfigTypeDef* config = RS485_GetConfig(rs485_port);

    if (config == 0 || data == 0 || size == 0)
    {
        return HAL_ERROR;
    }

    RS485_SetDirection(config, 1); // 设置为发送模式
    // 串口dma发送
    if (BSP_UART_Transmit_DMA(config->uart_port, 
                                data, 
                                size,
                                RS485_TransmitCompleteCallback,
                                config) != HAL_OK)
    {
        RS485_SetDirection(config, 0);  // 失败则恢复为接收模式
        return HAL_ERROR;
    }
    // 在发送完成回调中将方向设置回接收模式

    return HAL_OK;
}

HAL_StatusTypeDef RS485_Receive_DMA(RS485_PortTypeDef rs485_port,
                                    uint8_t *data,
                                    uint16_t size)
{
    BSP_RS485_ConfigTypeDef* config = RS485_GetConfig(rs485_port);

    if (config == 0 || data == 0 || size == 0 || config->rx_callback == 0)
    {
        return HAL_ERROR;
    }

    RS485_SetDirection(config, 0); // 设置为接收模式
    // 串口dma接收
    if (BSP_UART_StartReceiveIdle(config->uart_port,
                             data,
                             size,
                             RS485_ReceiveCompleteCallback,
                             config) != HAL_OK)
    {
        return HAL_ERROR;
    }
    return HAL_OK;
}


// 获取RS485配置结构体
static BSP_RS485_ConfigTypeDef* RS485_GetConfig(RS485_PortTypeDef rs485_port)
{
    for (int i = 0; i < RS485_PORT_MAX; i++)
    {
        if (rs485_config_tables[i].port == rs485_port)
        {
            return &rs485_config_tables[i];
        }
    }

    return 0;
}

// 设置RS485方向
static void RS485_SetDirection(BSP_RS485_ConfigTypeDef* config, uint8_t direction)
{
    if (direction)
    {
        HAL_GPIO_WritePin(config->de_port, config->de_pin, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(config->de_port, config->de_pin, GPIO_PIN_RESET);
    }
}

// 发送完成回调函数
static void RS485_TransmitCompleteCallback(void* context)
{
    BSP_RS485_ConfigTypeDef* config = (BSP_RS485_ConfigTypeDef*)context;

    if (config == 0 || config->tx_callback == 0)
    {
        return;
    }

    RS485_SetDirection(config, 0); // 设置为接收模式
    config->tx_callback(config->tx_context);
}
// 接收完成回调函数
static void RS485_ReceiveCompleteCallback(void* context)
{
    BSP_RS485_ConfigTypeDef* config = (BSP_RS485_ConfigTypeDef*)context;

    if (config == 0 || config->rx_callback == 0)
    {
        return;
    }

    config->rx_callback(config->rx_context);
}