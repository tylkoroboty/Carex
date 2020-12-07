/*
 * HC05.cpp
 *
 *  Created on: Nov 24, 2020
 *      Author: mikol
 */

#include "stm32f4xx_hal.h"

enum MODE:uint8_t {DATA, AT};
enum ROLE:uint8_t {SLAVE, MASTER};
enum STATUS:uint8_t {OK, ERR, WRONGARG};

class HC05{

public:
	void SetKey(uint16_t Pin, GPIO_TypeDef *Port);
	void SetUart(UART_HandleTypeDef *Uart);

	void DataSend(uint8_t *Data, uint8_t n);
	void DataReceive(uint8_t *Data, uint8_t n);

	STATUS SetMode(MODE);

	STATUS ATSetName(uint8_t *Name, uint8_t n);
	STATUS ATUartBaudrate(uint16_t Baudrate, uint8_t Parity, uint8_t Odd);

	STATUS ATSetRole(ROLE);
	STATUS ATPair(uint8_t *Address);
	STATUS ATInit();
	STATUS ATBind(uint8_t *Address);
	STATUS ATLink(uint8_t *Address);
	STATUS ATCmode(uint8_t Cmode);
	STATUS ATClearPairList();

	STATUS ATReset();

	void ATSetDefault();

protected:

	STATUS ATCheckOK();
	STATUS ATisActive();
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
	HAL_UART_Transmit(itsuart, Data, n, 100);
}

void HC05::DataReceive(uint8_t *Data, uint8_t n){
	HAL_UART_Receive(itsuart, Data, n, 500);
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

STATUS HC05::ATCheckOK(){
	uint8_t readData[20] = {0};
	uint8_t readDataLen = 20;

	uint8_t Response[] = "OK";

	DataReceive(readData, readDataLen);

	for(uint8_t i=0; i<2; i++)
	{
		if(readData[i] != Response[i]) return ERR;
	}

	return OK;
}

STATUS HC05::ATisActive(){
	uint8_t command[] = "AT";
	uint8_t commandLen = 2;

	DataSend(command, commandLen);
	ATEol();
	return ATCheckOK();
}

STATUS HC05::ATSetRole(ROLE Mode){

	uint8_t command[] = "AT+ROLE=";
	uint8_t commandLen = 8;
	uint8_t charRole = '0';

	if(Mode == SLAVE){
		DataSend(command, commandLen);
		DataSend(&charRole, 1);
		ATEol();
		if(ATCheckOK()) return ERR;
		itsRole = Mode;
		return OK;
	}

	if(Mode == MASTER){
		charRole = '1';
		DataSend(command, commandLen);
		DataSend(&charRole, 1);
		ATEol();
		if(ATCheckOK()) return ERR;
		itsRole = MASTER;
		return OK;
	}

	return WRONGARG;
}

STATUS HC05::SetMode(MODE mod){

	if(mod == AT)
	{
		KeyEnable();

		if(ATisActive())
			{
			KeyDisable();
			return ERR;
			}

		itsMode = mod;
		return OK;
	}
	if(mod == DATA){
		KeyDisable();
		itsMode = mod;
		return OK;
	}
	return WRONGARG;
}

STATUS HC05::ATSetName(uint8_t* name, uint8_t n){

	uint8_t command[] = "AT+NAME=";
	uint8_t commandLen = 8;

	DataSend(command, commandLen);
	DataSend(name, n);
	ATEol();

	return ATCheckOK();
}

STATUS HC05::ATReset(){

	uint8_t command[] = "AT+RESET";
	uint8_t commandLen = 8;

	DataSend(command, commandLen);
	ATEol();

	return ATCheckOK();

}

STATUS HC05::ATClearPairList(){

	uint8_t command[] = "AT+RMAAD";
	uint8_t commandLen = 8;

	DataSend(command, commandLen);
	ATEol();

	return ATCheckOK();
}

STATUS HC05::ATCmode(uint8_t Cmode){
	uint8_t command[] = "AT+CMODE=";
	uint8_t commandLen = 9;

	uint8_t cmodeChar = '0';
	if(Cmode!=0) cmodeChar = '1';

	DataSend(command, commandLen);
	DataSend(&cmodeChar, 1);
	ATEol();

	return ATCheckOK();
}

STATUS HC05::ATPair(uint8_t *Address){

	uint8_t command[] = "AT+PAIR=";
	uint8_t commandLen = 8;

	uint8_t pairDelay[] = ",5";
	uint8_t pairDelayLen = 2;

	uint8_t j=0;
	uint8_t comma = ',';
	DataSend(command, commandLen);

	DataSend(Address, 14);
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

	for(uint8_t i=0; i<15; i++){
		if(!ATCheckOK()) return OK;
		HAL_Delay(100);
	}

	return ERR;
}

STATUS HC05::ATInit(){
	uint8_t command[] = "AT+INIT";
	uint8_t commandLen = 7;

	DataSend(command, commandLen);
	ATEol();

	return OK;
}

STATUS HC05::ATBind(uint8_t *Address){
	uint8_t command[] = "AT+BIND=";
	uint8_t commandLen = 8;

	DataSend(command, commandLen);

	DataSend(Address, 14);
	ATEol();
	return ATCheckOK();
}

STATUS HC05::ATLink(uint8_t *Address){
	uint8_t command[] = "AT+LINK=";
	uint8_t commandLen = 8;

	DataSend(command, commandLen);

	DataSend(Address, 14);
	ATEol();

	for(uint8_t i=0; i<50; i++){
		if(!ATCheckOK()) return OK;
		HAL_Delay(10);
	}

	return ATCheckOK();
}
