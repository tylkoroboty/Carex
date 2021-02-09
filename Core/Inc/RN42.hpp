/*
 * RN42.hpp
 *
 *  Created on: Feb 8, 2021
 *      Author: mikol
 */
#include "stm32f4xx_hal.h"

enum STATUS: uint8_t {OK, ERR};

class BTmodule{
	public:
	STATUS Connect();
	STATUS Send();
	STATUS Receive();
};

class RN42: public BTmodule{
	public:
	STATUS Connect(char* BluetoothAddress);
	STATUS Send(uint8_t data);
	STATUS Send(uint8_t* data,int size);
	STATUS Receive(uint8_t* data);

	STATUS SetUART(UART_HandleTypeDef *huart);
	private:
	UART_HandleTypeDef *itsuart;
};
