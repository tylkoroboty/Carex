/*
 * RN42.cpp
 *
 *  Created on: Feb 8, 2021
 *      Author: mikol
 */

#include <RN42.hpp>
#include <bits/stdc++.h>

STATUS RN42::SetUART(UART_HandleTypeDef *huart){
	itsuart = huart;
	return OK;
}

STATUS RN42::Send(uint8_t* data, int size){
	int response;

	response=HAL_UART_Transmit(itsuart, data, size, 100);
	if(response == 0) return OK;
	else return ERR;
}

STATUS RN42::Connect(char * Address){
	char cmd[] = "$$$";
	char connect[] = "C,";
	char el[] = "\r\n";
	char end[] = "---\r\n";

	char dataToSend[30];

	std::strcpy(dataToSend,connect);
	std::strcat(dataToSend,Address);
	std::strcat(dataToSend,el);


	Send((uint8_t*)cmd,3);
	HAL_Delay(1000);
	Send((uint8_t*)dataToSend,16);
	HAL_Delay(7000);
	//Send((uint8_t*)end,5);

	return OK;
}
