//#ifndef __RS485_H
//#define __RS485_H

//#include "stm32f1xx_hal.h"  // 根据芯片型号修改

//#define RS485_UART_HANDLE   huart1
//#define RS485_DE_PORT       GPIOA
//#define RS485_DE_PIN        GPIO_PIN_8

//#define RS485_RX_BUF_SIZE   256
//#define RS485_TX_BUF_SIZE   256

//#define RS485_SET_TX()      HAL_GPIO_WritePin(RS485_DE_PORT, RS485_DE_PIN, GPIO_PIN_SET)
//#define RS485_SET_RX()      HAL_GPIO_WritePin(RS485_DE_PORT, RS485_DE_PIN, GPIO_PIN_RESET)

//extern UART_HandleTypeDef RS485_UART_HANDLE;

//void RS485_Init(void);
//HAL_StatusTypeDef RS485_Transmit_DMA(uint8_t *pData, uint16_t Size);
//HAL_StatusTypeDef RS485_Receive_DMA(uint8_t *pData, uint16_t Size);

//void RS485_UART_IRQHandler(void);
//void RS485_TxCpltCallback(void);
//void RS485_IdleCallback(void);

//uint16_t RS485_GetRxLen(void);
//uint8_t* RS485_GetRxBuf(void);

//#endif
