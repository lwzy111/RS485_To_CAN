//#ifndef BSP_USART_H
//#define BSP_USART_H

//#include "usart.h"

//#define BSP_UART_MAX_NUM 1

//typedef enum {
//    BSP_UART_RS485 = 0,
//    BSP_UART_NUM
//} bsp_uart_id_t;

//typedef enum {
//    BSP_UART_POLL = 0,
//    BSP_UART_IT,
//    BSP_UART_DMA    
//} bsp_uart_mode_t;

//typedef void (*BSP_UART_RxCpltCallback_t)(bsp_uart_id_t id, uint8_t *data, uint16_t size);

///**
// * @brief UART Handle Type Definition
// * @note This structure defines the handle for a specific UART instance.
// * */
//static UART_HandleTypeDef *const bsp_uart_handles[BSP_UART_NUM] = {
//		[BSP_UART_RS485] = &huart2
//};


///** 
// * @brief UART Status Type Definition 
// * @note This enumeration defines the possible status values for UART operations.
// * */
//typedef enum {
//    BSP_UART_OK = 0,    // Operation completed successfully
//    BSP_UART_ERROR,     // An error occurred during the operation
//    BSP_UART_BUSY       // The UART is currently busy with another operation
//} BSP_UART_StatusTypeDef;


//void BSP_UART_Init(void);

//BSP_UART_StatusTypeDef BSP_UART_Transmit(bsp_uart_id_t id, const uint8_t *data, uint16_t size, uint32_t timeout);

//BSP_UART_StatusTypeDef BSP_UART_Receive(bsp_uart_id_t id, uint8_t *data, uint16_t size, uint32_t timeout);

//BSP_UART_StatusTypeDef BSP_UART_Transmit_IT(bsp_uart_id_t id, const uint8_t *data, uint16_t size);

//BSP_UART_StatusTypeDef BSP_UART_Receive_IT(bsp_uart_id_t id, uint8_t *data, uint16_t size);

//BSP_UART_StatusTypeDef BSP_UART_Transmit_DMA(bsp_uart_id_t id, const uint8_t *data, uint16_t size);

//BSP_UART_StatusTypeDef BSP_UART_Receive_DMA(bsp_uart_id_t id, uint8_t *data, uint16_t size);


//#endif /* BSP_USART_H */