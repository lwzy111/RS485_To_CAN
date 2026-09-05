#ifndef __FSD13_H
#define __FSD13_H

#include "stm32f1xx_hal.h"

typedef enum
{
    FSD13_READY,
    FSD13_INIT,
    FSD13_LASER_ON,
    FSD13_LASER_OFF,
    FSD13_READ,
    FSD13_ZERO_SET,
    FSD13_ZERO_CLEAR,
    FSD13_SAVE,
    FSD13_TIMEOUT,
    FSD13_ERROR,
} FSD13_Status_t;


typedef enum
{
    FSD13_TEST,
    FSD13_2,
    FSD13_3,
    FSD13_MAX
} FSD13_Port_t;

void FSD13_Init(FSD13_Port_t port, uint8_t address);
void FSD13_LaserOn(FSD13_Port_t port);
void FSD13_LaserOff(FSD13_Port_t port);
void FSD13_StartRead(FSD13_Port_t port);
void FSD13_ZeroSet(FSD13_Port_t port);
void FSD13_ZeroClear(FSD13_Port_t port);
void FSD13_Save(FSD13_Port_t port);

void FSD13_ParseData(uint8_t port, uint8_t* data, uint16_t length);

float FSD13_GetDistance(FSD13_Port_t port);
FSD13_Status_t FSD13_GetStatus(FSD13_Port_t port);
void FSD13_SetStatus(FSD13_Port_t port, FSD13_Status_t status);
uint8_t FSD13_GetNewDataFlag(FSD13_Port_t port);
void FSD13_ClearNewDataFlag(FSD13_Port_t port);


#endif