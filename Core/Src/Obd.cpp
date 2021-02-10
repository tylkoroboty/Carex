/*
 * Obd.cpp
 *
 *  Created on: 5 gru 2020
 *      Author: mikol
 */
#include "stm32f4xx_hal.h"
#include <Obd.hpp>

uint8_t Obd::ASCII2HEX(uint8_t *tab){

	uint8_t A2H;
	uint8_t Hex=0;

	for(uint8_t i=0; i<2; i++){
		if(tab[i]>=0x30 && tab[i]<=0x39){
			//Cyfra z zakresu 0-9
			A2H=tab[i]-0x30;
		}
		else if(tab[i]>=0x41 && tab[i]<=0x46){
			//Cyfry z zakresu A-F
			A2H=tab[i]-0x37;
		}

	Hex+=(A2H << (4*(1-i)));
	}

	return Hex;
}

void Obd::SendData(BTmodule *BT,char* data, uint8_t n){
	BT->Send((uint8_t*) data, n);
}

void Obd::ReadData(BTmodule *BT,char* data, uint8_t n){
	BT->Receive((uint8_t*)data, n);
}

void Obd::Eol(BTmodule *BT){
	uint8_t CR = '\r';
	uint8_t LF = '\n';

	BT->Send(&CR, 1);
	BT->Send(&LF, 1);
}

uint16_t Obd::GetRPM(BTmodule *BT){
	char PID[] = "010C";
	uint8_t PIDLen = 4;

	int respLen = 2; //Response Lenght - 2bytes
	int respPrefixLen = 2; //Prefix ex. [41 0C]

	uint16_t Data[10];
	char tempByte;
	uint8_t tempData[2];
	int maxDatalen = 10;

	uint16_t rpm;

	SendData(BT, PID, PIDLen);
	Eol(BT);

	for(int byteN=0; byteN<(respLen+2); ++byteN){
		for(int tempDataN=0; tempDataN<=maxDatalen; ++tempDataN){
			ReadData(BT, &tempByte, 1);
			if(tempByte == ' ' || tempByte == '\r') break;
			tempData[0] = tempData[1];
			tempData[1] = tempByte;
			if(tempDataN == (maxDatalen-1)){
				return 0;
			}
		}
		Data[byteN]= ASCII2HEX(tempData);
	}

//	for(uint8_t byteN=0; byteN<byteReadN; ++byteN){
//		//Odbieranie bajtów z ELM
//
//		uint8_t tempData[2];
//		char tempByte;
//
//		for(uint8_t tempDataN=0; tempDataN<10; ++tempDataN){
//			ReadData(BT, &tempByte, 1);
//			if(tempByte == ' ') break;
//			tempData[0] = tempData[1];
//			tempData[1] = tempByte;
//		}
//		Data[byteN]= ASCII2HEX(tempData);
//	}
//
//	ReadData(BT, Data, 4);

	rpm = (256*Data[2]+Data[3])/4;

	return rpm;
}

uint8_t Obd::GetSpeed(BTmodule *BT){
	char PID[] = "010D";
	uint8_t PIDLen = 5;

	uint8_t speed;

	SendData(BT, PID, PIDLen);
	Eol(BT);
	//ReadData(BT, &speed, 1);

	return speed;
}
