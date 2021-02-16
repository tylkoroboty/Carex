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

	response=HAL_UART_Transmit(itsuart, data, size, 10);
	if(response == 0) return OK;
	else return ERR;
}

STATUS RN42::Receive(uint8_t *data, int size, uint32_t timeout){

	int resp=0;
	int errTimer = 1500*timeout;
	uint8_t readByte;

	char tempData[30];

	for(int i=0; i<size; ++i){
		while(__HAL_UART_GET_FLAG(itsuart,UART_FLAG_RXNE)==0 && errTimer>0){
			errTimer--;
		}
		if(errTimer==0) return ERR;
		tempData[i] = itsuart->Instance->DR;

		data[i] = tempData[i];
	}

	if(resp==0) return OK;
	return ERR;

}

STATUS RN42::Reset(){
	char cmd[] = "$$$";
	char cmdRESP[] = "CMD";
	char el[] = "\r\n";
	char end[] = "---\r\n";

	char kill[] = "K,1";
	char reboot[] = "R,1";
	char rebootRESP[] = "Reboot!";
	char recData[30];

	/* Send open CMD mode command >$$$ */
	Send((uint8_t*)cmd,3);
	/* Expect receive >CMD */
	Receive((uint8_t*)recData, 10, 1000);
	if(strncmp(recData, cmdRESP,3) != 0){
		Send((uint8_t*)end,5);
		Receive((uint8_t*)recData, 10, 1000);
		Send((uint8_t*)end,5);
		Receive((uint8_t*)recData, 10, 1000);
		return ERR;
	}

	Send((uint8_t*)kill,3);
	Send((uint8_t*)el,2);

	Receive((uint8_t*)recData, 10, 1000);
	HAL_Delay(100);

	Send((uint8_t*)reboot,3);
	Send((uint8_t*)el,2);

	Receive((uint8_t*)recData, 10, 1000);

	if(strncmp(recData,rebootRESP,7) == 0) return OK;

	return ERR;

}

STATUS RN42::Connect(char * Address){
	char cmd[] = "$$$";
	char cmdRESP[] = "CMD";
	char connect[] = "C,";
	char connectRESP[] = "TRYING";
	char el[] = "\r\n";
	char end[] = "---\r\n";

	char dataToSend[30];
	char recData[30];

	std::strcpy(dataToSend,connect);
	std::strcat(dataToSend,Address);
	std::strcat(dataToSend,el);

	/* Send open CMD mode command >$$$ */
	Send((uint8_t*)cmd,3);
	/* Expect receive >CMD */
	Receive((uint8_t*)recData, 10, 1000);
	if(strncmp(recData, cmdRESP,3) != 0){
		Send((uint8_t*)end,5);
		Receive((uint8_t*)recData, 10, 1000);
		Send((uint8_t*)end,5);
		Receive((uint8_t*)recData, 10, 1000);
		return ERR;
	}

	/* Send command C,<addr> and wait till connection established*/
	Send((uint8_t*)dataToSend,16);

	/* Expect receive TRYING */
	Receive((uint8_t*)recData, 30, 1000);
	HAL_Delay(7000);
	if(strncmp(recData,connectRESP,6) == 0) return OK;

	/* If not close CMD >---*/
	Send((uint8_t*)end,5);
	Receive((uint8_t*)recData, 10, 1000);

	return ERR;
}

STATUS RN42::IsConnected(){
	char cmd[] = "$$$";
	char cmdRESP[] = "CMD";

	char stat[] = "GK";
	char el[] = "\r\n";
	char end[] = "---\r\n";

	char recData[20];

	/* Send open CMD mode command >$$$ */
	Send((uint8_t*)cmd,3);
	/* Expect receive >CMD */
	Receive((uint8_t*)recData, 10, 1000);
	if(strncmp(recData, cmdRESP,3) != 0){
		Send((uint8_t*)end,5);
		Receive((uint8_t*)recData, 10, 1000);
		Send((uint8_t*)end,5);
		Receive((uint8_t*)recData, 10, 1000);
		return ERR;
	}

	/* Query for device current connection status >GK */
	Send((uint8_t*)stat,2);
	Send((uint8_t*)el,2);

	/* Experct answer >1,0,0 */
	Receive((uint8_t*)recData, 20, 2000);

	if(recData[0] == '1'){
		Send((uint8_t*)end,5);
		Receive((uint8_t*)recData, 10, 1000);
		return OK;
	}

	Send((uint8_t*)end,5);
	Receive((uint8_t*)recData, 10, 1000);

	return ERR;
}
