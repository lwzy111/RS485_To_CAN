//#include "rs485.h"
//#include <string.h>

//static uint8_t rs485_rx_buf[RS485_RX_BUF_SIZE];
//static uint8_t rs485_tx_buf[RS485_TX_BUF_SIZE];
//static volatile uint16_t rs485_rx_len = 0;
//static volatile uint8_t rs485_tx_busy = 0;

//void RS485_Init(void)
//{
//    RS485_SET_RX();
//}

//static void RS485_SetDirection_TX(void)
//{
//    RS485_SET_TX();
//    for (volatile uint32_t i = 0; i < 20; i++) __NOP();
//}

//static void RS485_SetDirection_RX(void)
//{
//    while (__HAL_UART_GET_FLAG(&RS485_UART_HANDLE, UART_FLAG_TC) == RESET);
//    RS485_SET_RX();
//}

//HAL_StatusTypeDef RS485_Transmit_DMA(uint8_t *pData, uint16_t Size)
//{
//    if (rs485_tx_busy) return HAL_BUSY;
//    if (Size == 0 || Size > RS485_TX_BUF_SIZE) return HAL_ERROR;

//    memcpy(rs485_tx_buf, pData, Size);
//    rs485_tx_busy = 1;

//    RS485_SetDirection_TX();
//    return HAL_UART_Transmit_DMA(&RS485_UART_HANDLE, rs485_tx_buf, Size);
//}

//HAL_StatusTypeDef RS485_Receive_DMA(uint8_t *pData, uint16_t Size)
//{
//    __HAL_UART_ENABLE_IT(&RS485_UART_HANDLE, UART_IT_IDLE);
//    return HAL_UART_Receive_DMA(&RS485_UART_HANDLE, pData, Size);
//}

//void RS485_UART_IRQHandler(void)
//{
//    if (__HAL_UART_GET_FLAG(&RS485_UART_HANDLE, UART_FLAG_IDLE) != RESET)
//    {
//        __HAL_UART_CLEAR_IDLEFLAG(&RS485_UART_HANDLE);
//        RS485_IdleCallback();
//    }
//    HAL_UART_IRQHandler(&RS485_UART_HANDLE);
//}

//void RS485_TxCpltCallback(void)
//{
//    rs485_tx_busy = 0;
//    RS485_SetDirection_RX();
//}

//void RS485_IdleCallback(void)
//{
//    if (RS485_UART_HANDLE.hdmarx != NULL)
//    {
//        HAL_UART_DMAStop(&RS485_UART_HANDLE);
//        rs485_rx_len = RS485_RX_BUF_SIZE - __HAL_DMA_GET_COUNTER(RS485_UART_HANDLE.hdmarx);
//        HAL_UART_Receive_DMA(&RS485_UART_HANDLE, rs485_rx_buf, RS485_RX_BUF_SIZE);
//    }
//}

//uint16_t RS485_GetRxLen(void)
//{
//    return rs485_rx_len;
//}

//uint8_t* RS485_GetRxBuf(void)
//{
//    return rs485_rx_buf;
//}
