#include "fsd13.h"
#include "bsp_rs485.h"
#include <string.h>

typedef struct 
{
    FSD13_Port_t FSD13_Port; // FSD13端口
    RS485_PortTypeDef rs485_port; // RS485端口
    uint8_t address; // 从机地址
    uint8_t new_data_flag; // 新数据标志
    int16_t rawdata; // 原始有效数据
    float distance_offset; // 距离偏移值，单位mm
    float distance;  // 距离值，单位mm
    FSD13_Status_t status;  // 状态信息

    uint8_t rx_buffer[128]; // 接收缓冲区
} FSD13_Config_t;

FSD13_Config_t fsd13_config[] = 
{
    {FSD13_TEST, RS485_PORT_2, 0, 0, 0, 0.0f, 0.0f, FSD13_READY, {0}},
};


static FSD13_Config_t* FSD13_GetConfig(FSD13_Port_t port);
static uint16_t CRC16_Modbus(uint8_t *buf, uint16_t len);


void FSD13_Init(FSD13_Port_t port, uint8_t address)
{
    FSD13_Config_t* config = FSD13_GetConfig(port);
    if (!config)
    {
        return;
    }
    // 保存地址
    config->address = address;

    // // 发送初始化命令
    // static uint8_t command[8];
    // memset(command, 0, sizeof(command));
    // command[0] = config->address;
    // command[1] = 0x05;
    // command[2] = 0x07;
    // command[3] = 0xD3;
    // command[4] = 0x00;
    // command[5] = 0x00;
    // uint16_t crc = CRC16_Modbus(command, 6);
    // command[6] = crc & 0xFF;
    // command[7] = (crc >> 8) & 0xFF;
    // RS485_Transmit_DMA(config->rs485_port, 
    //     command, 
    //     sizeof(command));
    // // 设置状态
    // fsd13_config[port].status = FSD13_INIT;
}

void FSD13_LaserOn(FSD13_Port_t port)
{
    FSD13_Config_t* config = FSD13_GetConfig(port);
    if (!config)
    {
        return;
    }
    // 发送激光开启命令
    static uint8_t command[8];
    memset(command, 0, sizeof(command));
    command[0] = config->address;
    command[1] = 0x05;
    command[2] = 0x07;
    command[3] = 0xD6;
    command[4] = 0xFF;
    command[5] = 0x00;
    uint16_t crc = CRC16_Modbus(command, 6);
    command[6] = crc & 0xFF;
    command[7] = (crc >> 8) & 0xFF;
    RS485_Transmit_DMA(config->rs485_port, 
        command, 
        sizeof(command));
    // 设置状态繁忙
    config->status = FSD13_LASER_ON;
}

void FSD13_LaserOff(FSD13_Port_t port)
{
    FSD13_Config_t* config = FSD13_GetConfig(port);
    if (!config)
    {
        return;
    }
    // 发送激光关闭命令
    static uint8_t command[8];
    memset(command, 0, sizeof(command));
    command[0] = config->address;
    command[1] = 0x05;
    command[2] = 0x07;
    command[3] = 0xD6;
    command[4] = 0x00;
    command[5] = 0x00;
    uint16_t crc = CRC16_Modbus(command, 6);
    command[6] = crc & 0xFF;
    command[7] = (crc >> 8) & 0xFF;
    RS485_Transmit_DMA(config->rs485_port, 
        command, 
        sizeof(command));
    // 设置状态繁忙
    config->status = FSD13_LASER_OFF;
}

void FSD13_StartRead(FSD13_Port_t port)
{
    FSD13_Config_t* config = FSD13_GetConfig(port);
    if (!config)
    {
        return;
    }
    // 发送读取命令
    static uint8_t command[8];
    memset(command, 0, sizeof(command));
    command[0] = config->address;
    command[1] = 0x04;
    command[2] = 0x00;
    command[3] = 0x64;
    command[4] = 0x00;
    command[5] = 0x01;
    uint16_t crc = CRC16_Modbus(command, 6);
    command[6] = crc & 0xFF;
    command[7] = (crc >> 8) & 0xFF;
    RS485_Transmit_DMA(config->rs485_port, 
        command, 
        sizeof(command));
    // 设置状态
    config->status = FSD13_READ;
}


// 解析Modbus读取响应
static void FSD13_ParseReadResponse(FSD13_Config_t* config, uint8_t* data)
{
    config->new_data_flag = 0;
    int16_t raw = (int16_t)(((uint16_t)data[3] << 8) | data[4]);

    // 0x7FFF表示超量程
    if (raw == 0x7FFF)
    {
        config->status = FSD13_ERROR;
        return;
    }

    // FSD13-200: 1 count = 0.01mm
    config->rawdata = raw;
    config->distance = 200.0f + (float)raw * 0.01f - config->distance_offset;
    config->status = FSD13_READY;
    config->new_data_flag = 1; // 设置新数据标志
}

// 解析Modbus写操作响应
static void FSD13_ParseWriteResponse(FSD13_Config_t* config, uint8_t* data)
{
    // 写操作响应为请求帧前6字节的回显，CRC已在外层校验
    if (data[1] != 0x05)
    {
        config->status = FSD13_ERROR;
        return;
    }

    // 对当前已实现的命令检查寄存器地址和写入值，避免旧响应被误当成当前命令的应答。
    uint16_t register_address = ((uint16_t)data[2] << 8) | data[3];
    uint16_t register_value = ((uint16_t)data[4] << 8) | data[5];
    if ((config->status == FSD13_INIT &&
         (register_address != 0x07D3U || register_value != 0x0000U)) ||
        (config->status == FSD13_LASER_ON &&
         (register_address != 0x07D6U || register_value != 0xFF00U)) ||
        (config->status == FSD13_LASER_OFF &&
         (register_address != 0x07D6U || register_value != 0x0000U)))
    {
        config->status = FSD13_ERROR;
        return;
    }

    config->status = FSD13_READY;
}

// 解析接收到的数据
void FSD13_ParseData(uint8_t port, uint8_t* data, uint16_t length)
{
    FSD13_Config_t* config = FSD13_GetConfig((FSD13_Port_t)port);
    uint16_t frame_length = 0;

    if (config == 0 || data == 0)
    {
        return;
    }

    // 清除新数据标志
    config->new_data_flag = 0;

    // 地址和功能码至少需要存在
    if (length < 2)
    {
        config->status = FSD13_ERROR;
        config->new_data_flag = 0;
        return;
    }

    // 非本传感器数据直接丢弃
    if (data[0] != config->address)
    {
        return;
    }


    // 根据功能码推导实际帧长，length只表示接收缓冲区可用容量。
    if (data[1] == 0x84 || data[1] == 0x85)
    {
        // 地址、异常功能码、异常码、CRC
        frame_length = 5;
    }
    else if (data[1] == 0x04)
    {
        // 地址、功能码、字节数、数据、CRC
        if (length < 3)
        {
            config->status = FSD13_ERROR;
            return;
        }
        frame_length = (uint16_t)data[2] + 5U;
        if (data[2] != 2U)
        {
            config->status = FSD13_ERROR;
            return;
        }
    }
    else if (data[1] == 0x05)
    {
        // 地址、功能码、寄存器地址、写入值、CRC
        frame_length = 8;
    }
    else
    {
        config->status = FSD13_ERROR;
        return;
    }

    // 防止按推导长度访问接收缓冲区之外的数据。
    if (frame_length > length)
    {
        config->status = FSD13_ERROR;
        return;
    }

    // Modbus CRC低字节在前，高字节在后
    uint16_t crc_calc = CRC16_Modbus(data, frame_length - 2);
    uint16_t crc_recv = (uint16_t)data[frame_length - 2] |
                        ((uint16_t)data[frame_length - 1] << 8);
    if (crc_calc != crc_recv)
    {
        config->status = FSD13_ERROR;
        return;
    }

    if (data[1] == 0x04)
    {
        FSD13_ParseReadResponse(config, data);
    }
    else if (data[1] == 0x05)
    {
        FSD13_ParseWriteResponse(config, data);
    }
    else
    {
        config->status = FSD13_ERROR;
    }
}



// 设置状态
void FSD13_SetStatus(FSD13_Port_t port, FSD13_Status_t status)
{
    FSD13_Config_t* config = FSD13_GetConfig(port);
    if (!config)
    {
        return;
    }
    config->status = status;
}

// 获取距离
float FSD13_GetDistance(FSD13_Port_t port)
{
    FSD13_Config_t* config = FSD13_GetConfig(port);
    if (!config)
    {
        return -1.0f;
    }
    return config->distance;
}

// 获取状态
FSD13_Status_t FSD13_GetStatus(FSD13_Port_t port)
{
    FSD13_Config_t* config = FSD13_GetConfig(port);
    if (!config)
    {
        return FSD13_ERROR;
    }
    return config->status;
}

// 获取新数据标志
uint8_t FSD13_GetNewDataFlag(FSD13_Port_t port)
{
    FSD13_Config_t* config = FSD13_GetConfig(port);
    if (!config)
    {
        return 0;
    }
    return config->new_data_flag;
}

// 清除新数据标志
void FSD13_ClearNewDataFlag(FSD13_Port_t port)
{
    FSD13_Config_t* config = FSD13_GetConfig(port);
    if (!config)
    {
        return;
    }
    config->new_data_flag = 0;
}



// 获取FSD13结构体
static FSD13_Config_t* FSD13_GetConfig(FSD13_Port_t port)
{
    for (int i = 0; i < sizeof(fsd13_config)/sizeof(FSD13_Config_t); i++)
    {
        if (fsd13_config[i].FSD13_Port == port)
        {
            return &fsd13_config[i];
        }
    }
    return 0;
}


// Modbus RTU CRC16 计算
static uint16_t CRC16_Modbus(uint8_t *buf, uint16_t len)
{
    uint16_t crc = 0xFFFF;
    for (uint16_t i = 0; i < len; i++) {
        crc ^= buf[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}
