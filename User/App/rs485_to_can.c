#include "rs485_to_can.h"

#include "bsp_rs485.h"
#include "can.h"
#include "fsd13.h"
#include "Can_Config.h"

#include "main.h"
#include "stm32f1xx_hal.h"
#include <string.h>

// 状态机
typedef enum
{
    FSD13_TEST_TASK,
    CAN_SEND,
} RS485_To_CAN_State_t;
RS485_To_CAN_State_t rs485_to_can_state = FSD13_TEST_TASK;


// RS485收发完成标志
volatile static uint8_t rs485_tx_over = 1;
volatile static uint8_t rs485_rx_over = 1;
// RS485接收缓冲区
static uint8_t rx_data[128];

// test distance
static uint16_t distance;


static void RS485_TransmitCompleteCallback(void* context)
{
    // 设置RS485发送完成标志
    rs485_tx_over = 1;
}

static void RS485_ReceiveCompleteCallback(void* context)
{
    // 设置RS485接收完成标志
    rs485_rx_over = 1;
    RS485_Receive_DMA(RS485_PORT_2, rx_data, sizeof(rx_data));
}


void RS485_To_CAN_Init(void)
{
    // 初始化CAN
    CAN_Init(&hcan);
    // 使能CAN引脚
    HAL_GPIO_WritePin(CAN_STB_GPIO_Port, CAN_STB_Pin, GPIO_PIN_RESET);
    // 注册RS485回调函数
    RS485_RegisterCallback(RS485_PORT_2, 
        RS485_TransmitCompleteCallback, 
        RS485_ReceiveCompleteCallback);
    // 初始化FSD13
    RS485_Receive_DMA(RS485_PORT_2, rx_data, sizeof(rx_data));
    FSD13_Init(FSD13_TEST, 0x01);
    // while (FSD13_GetStatus(FSD13_TEST) != FSD13_READY)
    // {
    //     if (FSD13_GetNewDataFlag(FSD13_TEST) == 1)
    //     {
    //         // 处理接收到的数据
    //         FSD13_ParseData(FSD13_TEST, rx_data, 7);
    //         FSD13_ClearNewDataFlag(FSD13_TEST); // 清除新数据标志
    //     }
    //     HAL_Delay(1);
    // }
    // RS485_Receive_DMA(RS485_PORT_2, rx_data, sizeof(rx_data));
    // // 打开FSD13激光
    // FSD13_LaserOn(FSD13_TEST);
    // while (FSD13_GetStatus(FSD13_TEST) != FSD13_READY)
    // {
    //     if (FSD13_GetNewDataFlag(FSD13_TEST) == 1)
    //     {
    //         // 处理接收到的数据
    //         FSD13_ParseData(FSD13_TEST, rx_data, sizeof(rx_data));
    //         FSD13_ClearNewDataFlag(FSD13_TEST); // 清除新数据标志
    //     }
    //     HAL_Delay(1);
    // }

    
}


void RS485_To_CAN_TASK(void)
{
    
    switch (rs485_to_can_state)
    {
        case FSD13_TEST_TASK:
            uint8_t new_data_flag = FSD13_GetNewDataFlag(FSD13_TEST);
            // 读取FSD13数据
            if (new_data_flag == 0 && rs485_tx_over && rs485_rx_over)
            {
                rs485_tx_over = 0;
                rs485_rx_over = 0;
                FSD13_StartRead(FSD13_TEST);
            }
            if (rs485_rx_over)
            {
                // 处理接收到的数据
                FSD13_ParseData(FSD13_TEST, rx_data, 7);
                FSD13_ClearNewDataFlag(FSD13_TEST); // 清除新数据标志
            }
            // 处理数据后迅速进入下一个状态
            if (new_data_flag == 0 && rs485_rx_over)
            {
                // 进入下一个状态
                rs485_to_can_state = CAN_SEND;
            }
            break;

        case CAN_SEND:
            // 打包can数据
            uint8_t can_data[8] = {0};
            memset(can_data, 0, sizeof(can_data));
            // test
            distance = (uint16_t)(10 * FSD13_GetDistance(FSD13_TEST));
            can_data[0] = (uint8_t)(distance & 0xFF);
            can_data[1] = (uint8_t)((distance >> 8) & 0xFF);
            // 发送can数据
            Can_Send_Data(&hcan, 0x123, can_data, 8);
            // 进入下一个状态
            rs485_to_can_state = FSD13_TEST_TASK;
            // test
            HAL_Delay(10);
            break;


        default:
            break;
    }
    // 例如：接收RS485数据后，通过CAN发送出去
}

