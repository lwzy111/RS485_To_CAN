/**
 * @file  fsd13_rs485.c
 * @brief  FSD13-RS485 激光位移传感器驱动实现
 */

#include "fsd13_rs485.h"
#include "string.h"

/* 私有变量 */
static FSD13_Model_t g_model = FSD13_30;
static FSD13_Protocol_t g_protocol = PROTOCOL_MODBUS;
static uint16_t g_center_distance = 30;  // 中心距离mm
static uint16_t g_resolution = 1;        // 分辨率 μm

/* Modbus RTU 常用指令（从站地址0x01） */
static const uint8_t MODBUS_READ_DIST[]  = {0x01, 0x04, 0x00, 0x64, 0x00, 0x01};
static const uint8_t MODBUS_LASER_ON[]   = {0x01, 0x05, 0x07, 0xD6, 0xFF, 0x00};
static const uint8_t MODBUS_LASER_OFF[]  = {0x01, 0x05, 0x07, 0xD6, 0x00, 0x00};
static const uint8_t MODBUS_ZERO_SET[]   = {0x01, 0x05, 0x07, 0xD7, 0xFF, 0x00};
static const uint8_t MODBUS_ZERO_CANCEL[]= {0x01, 0x05, 0x07, 0xD7, 0x00, 0x00};
static const uint8_t MODBUS_INIT[]       = {0x01, 0x05, 0x07, 0xD3, 0x00, 0x00};

/* 自定义协议 常用指令 */
#define CUSTOM_CMD_READ     0x43
#define CUSTOM_CMD_WRITE    0x43
#define CUSTOM_CMD_SAVE     0x43
#define CUSTOM_CMD_INIT     0x40

#define CUSTOM_DAT_READ     0xB0
#define CUSTOM_DAT_LASER_ON 0xA0
#define CUSTOM_DAT_LASER_OFF 0xA0
#define CUSTOM_DAT_ZERO_SET 0xA1
#define CUSTOM_DAT_ZERO_CANCEL 0xA1
#define CUSTOM_DAT_INIT     0x40

#define CUSTOM_SUB_READ     0x01
#define CUSTOM_SUB_LASER_ON 0x03
#define CUSTOM_SUB_LASER_OFF 0x02
#define CUSTOM_SUB_ZERO_SET 0x00
#define CUSTOM_SUB_ZERO_CANCEL 0x01
#define CUSTOM_SUB_INIT     0x00

#define CUSTOM_ETX          0x03

/* ===================== 私有函数 ===================== */

/**
 * @brief RS485方向控制
 * @param dir 1=发送, 0=接收
 */
static void RS485_SetDir(uint8_t dir)
{
    if(dir) {
        HAL_GPIO_WritePin(RS485_DE_PORT, RS485_DE_PIN, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(RS485_DE_PORT, RS485_DE_PIN, GPIO_PIN_RESET);
    }
}

/**
 * @brief Modbus RTU CRC16计算
 */
static uint16_t Modbus_CRC16(uint8_t *data, uint16_t len)
{
    uint16_t crc = 0xFFFF;
    for(uint16_t i = 0; i < len; i++) {
        crc ^= data[i];
        for(uint8_t j = 0; j < 8; j++) {
            if(crc & 0x0001) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

/**
 * @brief 自定义协议BCC计算（命令^数据1^数据2）
 */
static uint8_t Custom_BCC(uint8_t cmd, uint8_t dat1, uint8_t dat2)
{
    return cmd ^ dat1 ^ dat2;
}

/**
 * @brief Modbus发送指令并接收响应
 * @param tx_buf 发送缓冲区
 * @param tx_len 发送长度
 * @param rx_buf 接收缓冲区
 * @param rx_len 期望接收长度
 * @param timeout 超时时间ms
 * @return 0=成功, 1=失败
 */
static uint8_t Modbus_SendReceive(uint8_t *tx_buf, uint16_t tx_len, 
                                   uint8_t *rx_buf, uint16_t rx_len, 
                                   uint32_t timeout)
{
    uint16_t crc;
    uint8_t tx_full[32];
    
    if(tx_len + 2 > 32) return 1;
    
    memcpy(tx_full, tx_buf, tx_len);
    crc = Modbus_CRC16(tx_buf, tx_len);
    tx_full[tx_len] = crc & 0xFF;
    tx_full[tx_len + 1] = (crc >> 8) & 0xFF;
    
    RS485_SetDir(1);
    HAL_UART_Transmit(&FSD13_UART, tx_full, tx_len + 2, timeout);
    RS485_SetDir(0);
    
    // 等待传感器响应
    HAL_StatusTypeDef status = HAL_UART_Receive(&FSD13_UART, rx_buf, rx_len, timeout);
    
    if(status != HAL_OK) return 1;
    
    // 简单校验：检查从机地址
    if(rx_buf[0] != FSD13_SLAVE_ADDR) return 1;
    
    return 0;
}

/**
 * @brief 自定义协议发送指令并接收响应
 */
static uint8_t Custom_SendReceive(uint8_t cmd, uint8_t dat1, uint8_t dat2,
                                   uint8_t *rx_buf, uint16_t rx_len, 
                                   uint32_t timeout)
{
    uint8_t tx_buf[6];
    uint8_t bcc = Custom_BCC(cmd, dat1, dat2);
    
    tx_buf[0] = FSD13_CUSTOM_STX;   // STX
    tx_buf[1] = cmd;                // 命令
    tx_buf[2] = dat1;               // 数据1
    tx_buf[3] = dat2;               // 数据2
    tx_buf[4] = CUSTOM_ETX;         // ETX
    tx_buf[5] = bcc;                // BCC
    
    RS485_SetDir(1);
    HAL_UART_Transmit(&FSD13_UART, tx_buf, 6, timeout);
    RS485_SetDir(0);
    
    HAL_StatusTypeDef status = HAL_UART_Receive(&FSD13_UART, rx_buf, rx_len, timeout);
    
    if(status != HAL_OK) return 1;
    
    // 校验STX和ETX
    if(rx_buf[0] != FSD13_CUSTOM_STX) return 1;
    if(rx_buf[rx_len - 2] != CUSTOM_ETX) return 1;
    
    return 0;
}

/**
 * @brief 解析测量值（16位有符号数）
 * @param raw_data 原始数据（2字节，高位在前）
 * @return 带符号的原始值X
 */
static int16_t Parse_Measurement(uint8_t *raw_data)
{
    int16_t value = (raw_data[0] << 8) | raw_data[1];
    return value;
}

/* ===================== 公有函数 ===================== */

/**
 * @brief 初始化传感器驱动
 * @param model 传感器型号
 * @param protocol 通信协议
 */
void FSD13_Init(FSD13_Model_t model, FSD13_Protocol_t protocol)
{
    g_model = model;
    g_protocol = protocol;
    
    // 根据型号设置参数
    switch(model) {
        case FSD13_30:
            g_center_distance = 30;
            g_resolution = 1;
            break;
        case FSD13_50:
            g_center_distance = 50;
            g_resolution = 10;
            break;
        case FSD13_100:
            g_center_distance = 100;
            g_resolution = 10;
            break;
        case FSD13_200:
            g_center_distance = 200;
            g_resolution = 10;
            break;
        case FSD13_400:
            g_center_distance = 400;
            g_resolution = 100;
            break;
    }
    
    // 初始化DE引脚
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStruct.Pin = RS485_DE_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(RS485_DE_PORT, &GPIO_InitStruct);
    RS485_SetDir(0);
}

/**
 * @brief 读取距离值（单位：mm）
 * @param distance_mm 输出参数，距离值
 * @return 0=成功, 1=失败
 */
uint8_t FSD13_ReadDistance(float *distance_mm)
{
    int16_t raw_value;
    uint8_t rx_buf[16];
    uint8_t ret;
    
    if(g_protocol == PROTOCOL_MODBUS) {
        // Modbus: 01 04 00 64 00 01 + CRC
        ret = Modbus_SendReceive((uint8_t*)MODBUS_READ_DIST, 6, rx_buf, 7, 200);
        if(ret != 0) return 1;
        
        // 响应格式: 01 04 02 XX XX CRC CRC
        // 第3、4字节为测量值（高位在前）
        raw_value = Parse_Measurement(&rx_buf[3]);
        
    } else {
        // 自定义协议: 02 43 B0 01 03 BCC
        ret = Custom_SendReceive(CUSTOM_CMD_READ, CUSTOM_DAT_READ, CUSTOM_SUB_READ,
                                  rx_buf, 6, 200);
        if(ret != 0) return 1;
        
        // 响应格式: 02 06 XX XX 03 XX
        // 第3、4字节为测量值（高位在前）
        raw_value = Parse_Measurement(&rx_buf[2]);
    }
    
    // 换算为实际距离（mm）
    // 实际距离值 = 中心距离 ± X * 分辨率 / 1000
    float offset = (float)raw_value * g_resolution / 1000.0f;
    *distance_mm = (float)g_center_distance + offset;
    
    return 0;
}

/**
 * @brief 打开激光
 */
uint8_t FSD13_LaserOn(void)
{
    uint8_t rx_buf[16];
    
    if(g_protocol == PROTOCOL_MODBUS) {
        return Modbus_SendReceive((uint8_t*)MODBUS_LASER_ON, 6, rx_buf, 8, 200);
    } else {
        return Custom_SendReceive(CUSTOM_DAT_LASER_ON, CUSTOM_DAT_LASER_ON, CUSTOM_SUB_LASER_ON,
                                   rx_buf, 6, 200);
    }
}

/**
 * @brief 关闭激光
 */
uint8_t FSD13_LaserOff(void)
{
    uint8_t rx_buf[16];
    
    if(g_protocol == PROTOCOL_MODBUS) {
        return Modbus_SendReceive((uint8_t*)MODBUS_LASER_OFF, 6, rx_buf, 8, 200);
    } else {
        return Custom_SendReceive(CUSTOM_DAT_LASER_OFF, CUSTOM_DAT_LASER_OFF, CUSTOM_SUB_LASER_OFF,
                                   rx_buf, 6, 200);
    }
}

/**
 * @brief 归零设定
 */
uint8_t FSD13_ZeroSet(void)
{
    uint8_t rx_buf[16];
    
    if(g_protocol == PROTOCOL_MODBUS) {
        return Modbus_SendReceive((uint8_t*)MODBUS_ZERO_SET, 6, rx_buf, 8, 200);
    } else {
        return Custom_SendReceive(CUSTOM_DAT_ZERO_SET, CUSTOM_DAT_ZERO_SET, CUSTOM_SUB_ZERO_SET,
                                   rx_buf, 6, 200);
    }
}

/**
 * @brief 取消归零
 */
uint8_t FSD13_ZeroCancel(void)
{
    uint8_t rx_buf[16];
    
    if(g_protocol == PROTOCOL_MODBUS) {
        return Modbus_SendReceive((uint8_t*)MODBUS_ZERO_CANCEL, 6, rx_buf, 8, 200);
    } else {
        return Custom_SendReceive(CUSTOM_DAT_ZERO_CANCEL, CUSTOM_DAT_ZERO_CANCEL, CUSTOM_SUB_ZERO_CANCEL,
                                   rx_buf, 6, 200);
    }
}

/**
 * @brief 传感器初始化（恢复出厂设置）
 * @note  发送后还需要保存指令才能永久生效
 */
uint8_t FSD13_InitSensor(void)
{
    uint8_t rx_buf[16];
    
    if(g_protocol == PROTOCOL_MODBUS) {
        return Modbus_SendReceive((uint8_t*)MODBUS_INIT, 6, rx_buf, 8, 200);
    } else {
        return Custom_SendReceive(CUSTOM_CMD_INIT, CUSTOM_DAT_INIT, CUSTOM_SUB_INIT,
                                   rx_buf, 6, 200);
    }
}