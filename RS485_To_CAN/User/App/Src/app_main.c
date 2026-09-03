#include "app_main.h"

ADS1248_Handle_t ADS1248_dev1;
ADS1248_Handle_t ADS1248_dev2;

void App_Init(void) {

    ADS1248_InitHandle(&ADS1248_dev1, SPI1_DRDY_GPIO_Port, SPI1_DRDY_Pin, SPI1_CS_GPIO_Port, SPI1_CS_Pin, &hspi1);
    ADS1248_InitHandle(&ADS1248_dev2, SPI2_DRDY_GPIO_Port, SPI2_DRDY_Pin, SPI2_CS_GPIO_Port, SPI2_CS_Pin, &hspi2);

    ADS1248_StartInit(&ADS1248_dev1);
    ADS1248_StartInit(&ADS1248_dev2);
}

void App_Loop(void) {
    
}