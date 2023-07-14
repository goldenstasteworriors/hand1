#ifndef _RS485_DRV_H_
#define _RS485_DRV_H_

#include "usart.h"

/* User config */

#define RS485_USART         huart2
#define RS485_RE_GPIO_PORT  GPIOG
#define RS485_RE_GPIO_PIN GPIO_PIN_8

/* RS485 TX/RX CONTRL */
#define RS485_TX_ENABLE()		HAL_Delay(1);\
														HAL_GPIO_WritePin(RS485_RE_GPIO_PORT, RS485_RE_GPIO_PIN, GPIO_PIN_SET);\
														HAL_Delay(1);
#define RS485_RX_ENABLE()		HAL_Delay(1);\
														HAL_GPIO_WritePin(RS485_RE_GPIO_PORT, RS485_RE_GPIO_PIN, GPIO_PIN_RESET);\
														HAL_Delay(1);

void RS485_Init(void);
int RS485_Transmit(uint8_t *data, uint16_t len);

#endif /* _RS485_DVR_H_   */