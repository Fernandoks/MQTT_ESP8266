/*
 * task.h
 *
 *  Created on: Mar 31, 2020
 *      Author: fernandoks
 */

#ifndef INC_MYTASKS_H_
#define INC_MYTASKS_H_

#include "stm32f4xx_hal.h"


void StartDefaulttask(void const * argument);
void StartDeviceTask(void const * argument);
void StartPCTask(void const * argument);
void StartCommandtask(void const * argument);


void UART_write(UART_HandleTypeDef *huart, uint8_t* pdata, uint16_t Size);

#endif /* INC_MYTASKS_H_ */
