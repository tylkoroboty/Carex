/*
 * HC05.cpp
 *
 *  Created on: Nov 24, 2020
 *      Author: mikol
 */

#include "stm32f4xx_hal.h"

enum MODE:uint8_t {DATA, AT};
enum ROLE:uint8_t {SLAVE, MASTER};

#define ComAT "AT"

class HC05{

public:
	void SetKey(uint16_t Pin, GPIO_TypeDef *Port);
	void SetUart(UART_HandleTypeDef *Uart);

	void DataSend(uint8_t *Data, uint8_t n);
	void DataReceive(uint8_t *Data, uint8_t n);

	uint8_t SetMode(MODE);

	void ATSetName(uint8_t *Name, uint8_t n);
	void ATUartBaudrate(uint16_t Baudrate, uint8_t Parity, uint8_t Odd);

	void ATSetRole(ROLE);
	void ATPair(uint8_t *Address);

	void ATReset();



	void ATSetDefault();

protected:

	uint8_t ATisActive();
	void KeyEnable();
	void KeyDisable();
	void ATEol();

	/* KEY Pin */
	uint16_t itskeyPin;
	GPIO_TypeDef *itskeyPort;

	/* UART */
	UART_HandleTypeDef *itsuart;

	uint8_t itsRole = 0;
	uint8_t itsMode = AT;
};


void HC05::SetKey(uint16_t Pin, GPIO_TypeDef *Port){
	itskeyPin = Pin;
	itskeyPort = Port;

}

void HC05::SetUart(UART_HandleTypeDef *Uart){
	itsuart = Uart;

}

void HC05::DataSend(uint8_t *Data, uint8_t n){
	HAL_UART_Transmit(itsuart, Data, n, 10);
}

void HC05::DataReceive(uint8_t *Data, uint8_t n){
	HAL_UART_Receive(itsuart, Data, n, 100);
}

void HC05::KeyEnable(){

	HAL_GPIO_WritePin(itskeyPort, itskeyPin, GPIO_PIN_SET);
}

void HC05::KeyDisable(){
	HAL_GPIO_WritePin(itskeyPort, itskeyPin, GPIO_PIN_RESET);
}

void HC05::ATEol(){
	uint8_t CR = '\r';
	uint8_t LF = '\n';

	DataSend(&CR, 1);
	DataSend(&LF, 1);
}

uint8_t HC05::ATisActive(){
	uint8_t command[] = "AT";
	uint8_t commandLen = 2;

	uint8_t readData[6] = {0};
	uint8_t Response[3] = "OK";

	DataSend(command, commandLen);
	ATEol();

	DataReceive(readData, 6);

	for(uint8_t i=0; i<2; i++)
	{
		if(readData[i] != Response[i]) return 1;
	}
	return 0;
}

void HC05::ATSetRole(ROLE Mode){

	uint8_t command[] = "AT+ROLE=";
	uint8_t commandLen = 8;
	uint8_t charRole = '0';

	if(Mode == SLAVE){
		itsRole = Mode;
		DataSend(command, commandLen);
		DataSend(&charRole, 1);
		ATEol();
	}

	if(Mode == MASTER){
		itsRole = MASTER;
		charRole = '1';
		DataSend(command, commandLen);
		DataSend(&charRole, 1);
		ATEol();
	}
}

uint8_t HC05::SetMode(MODE mod){

	if(mod == AT)
	{
		KeyEnable();

		if(ATisActive())
			{
			KeyDisable();
			return 1;
			}

		itsMode = mod;
		return 0;
	}
	if(mod == DATA){
		KeyDisable();
		itsMode = mod;
		return 0;
	}
	return 2;
}

void HC05::ATSetName(uint8_t* name, uint8_t n){

	uint8_t command[] = "AT+NAME=";
	uint8_t commandLen = 8;

	DataSend(command, commandLen);
	DataSend(name, n);
	ATEol();
}

void HC05::ATReset(){

	uint8_t command[] = "AT+RESET";
	uint8_t commandLen = 8;

	SetMode(DATA);

	if(!ATisActive()){
		DataSend(command, commandLen);
		ATEol();
	}
}

void HC05::ATPair(uint8_t *Address){

	uint8_t command[] = "AT+PAIR=";
	uint8_t commandLen = 8;

	uint8_t pairDelay[] = ",10";
	uint8_t pairDelayLen = 3;

	uint8_t j=0;
	uint8_t comma = ',';
	DataSend(command, commandLen);

	DataSend(Address, 16);
	DataSend(pairDelay, pairDelayLen);
//	for(uint8_t i=0; i<14; i++){
//
//		if(i!=4 && i!=7){
//			DataSend(&Address[j], 1);
//			++j;
//		}
//		DataSend(&comma, 1);
//	}
	ATEol();
}
