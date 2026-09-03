//#include "bsp_uart.h"

////void BSP_USART_Init(void) {
////    // Initialize the UART peripheral
////    HAL_UART_Init(&UART_HANDLE);
////}

//BSP_UART_StatusTypeDef BSP_UART_Transmit(bsp_uart_id_t id, const uint8_t *data, uint16_t size, uint32_t timeout) {
//    if (id >= BSP_UART_NUM || bsp_uart_handles[id] == NULL) {
//        return BSP_UART_ERROR; // Invalid UART ID
//    }
//    if (HAL_UART_Transmit(bsp_uart_handles[id], (uint8_t *)data, size, timeout) == HAL_OK) {
//        return BSP_UART_OK;
//    } else {
//        return BSP_UART_ERROR;
//    }
//}

//BSP_UART_StatusTypeDef BSP_UART_Receive(bsp_uart_id_t id, uint8_t *data, uint16_t size, uint32_t timeout) {
//    if (id >= BSP_UART_NUM || bsp_uart_handles[id] == NULL) {
//        return BSP_UART_ERROR; // Invalid UART ID
//    }
//    if (HAL_UART_Receive(bsp_uart_handles[id], data, size, timeout) == HAL_OK) {
//        return BSP_UART_OK;
//    } else {
//        return BSP_UART_ERROR;
//    }
//}

//BSP_UART_StatusTypeDef BSP_UART_Transmit_DMA(bsp_uart_id_t id, const uint8_t *data, uint16_t size) {
//    if (id >= BSP_UART_NUM || bsp_uart_handles[id] == NULL) {
//        return BSP_UART_ERROR; // Invalid UART ID
//    }
//    if (HAL_UART_Transmit(bsp_uart_handles[id], (uint8_t *)data, size) == HAL_OK) {
//        return BSP_UART_OK;
//    } else {
//        return BSP_UART_ERROR;
//    }
//}

//BSP_UART_StatusTypeDef BSP_UART_Receive_DMA(bsp_uart_id_t id, uint8_t *data, uint16_t size) {
//    if (id >= BSP_UART_NUM || bsp_uart_handles[id] == NULL) {
//        return BSP_UART_ERROR; // Invalid UART ID
//    }
//    if (HAL_UART_Receive(bsp_uart_handles[id], data, size) == HAL_OK) {
//        return BSP_UART_OK;
//    } else {
//        return BSP_UART_ERROR;
//    }
//}


