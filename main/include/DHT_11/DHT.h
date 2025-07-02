#ifndef DHT11_H
#define DHT11_H

#include "stm32h7xx_hal.h"  // à adapter selon ta série STM32
#include "tim.h"
#include <stdint.h>

// Encapsulation pour compatibilité C++
#ifdef __cplusplus
extern "C" {
#endif

// Définition du port et pin utilisés
#define DHT11_PORT GPIOB
#define DHT11_PIN  GPIO_PIN_9

// Variables globales pour les mesures
extern uint8_t RHI, RHD, TCI, TCD, SUM;
extern int tCelsius, tFahrenheit;
extern float RH;

// Prototype des fonctions
void microDelay(uint16_t delay);
void milliDelay(uint16_t ms);
uint8_t DHT11_Start(void);
uint8_t DHT11_Read(void);

// Fin de encapsulation C++
#ifdef __cplusplus
}
#endif

#endif // DHT11_H
