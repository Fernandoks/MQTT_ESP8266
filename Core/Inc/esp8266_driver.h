/*
 * esp8266_driver.h
 *
 *  Created on: Apr 3, 2020
 *      Author: fernandoks
 */

#ifndef INC_ESP8266_DRIVER_H_
#define INC_ESP8266_DRIVER_H_

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#define ESP8266_ERROR      (-1)
#define ESP8266_INPROGRESS (0)
#define ESP8266_SUCCESS    (1)




typedef int32_t ESP8266_t;

ESP8266_t esp_command(uint8_t* command, uint32_t command_size);


#endif /* INC_ESP8266_DRIVER_H_ */
