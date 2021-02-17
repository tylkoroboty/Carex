/*
 * Obd.cpp
 *
 *  Created on: 5 gru 2020
 *      Author: mikol
 */
#include "stm32f4xx_hal.h"
#include <Obd.hpp>
#include <string.h>

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

void Obd::ReadData(BTmodule *BT,char* data, uint8_t n, uint32_t timeout){
	BT->Receive((uint8_t*)data, n, timeout);
}

void Obd::Eol(BTmodule *BT){
	uint8_t CR = '\r';
	uint8_t LF = '\n';

	BT->Send(&CR, 1);
	BT->Send(&LF, 1);
}

uint16_t Obd::GetRPM(BTmodule *BT){
	char PID[] = "010C";
	char Searching[] = "SEARCHING...";
	uint8_t PIDLen = 4;

	int word,letter;

//	int respLen = 2; //Response Lenght - 2bytes
//	int respPrefixLen = 2; //Prefix ex. [41 0C]

//	uint16_t Data[10];
//	char tempByte;
//	uint8_t tempData[2];

//	uint16_t rpm;
	uint16_t rpmH,rpmL;

	SendData(BT, PID, PIDLen);
	Eol(BT);

	const int maxWordNum = 10;
	const int maxLetterNum = 30;
	char rWord[maxWordNum][maxLetterNum];

	char rData[50];
	int index = 0;

	SendData(BT, PID, PIDLen);
	Eol(BT);

	ReadData(BT,rData,30,400);

	/* Dividing read data on words */
	for(word=0; word<maxWordNum; ++word){
		for(letter=0; letter<maxLetterNum; ++letter){
			if(rData[index] == ' ' || rData[index] == '>' || rData[index] == '\r') break;
			rWord[word][letter]=rData[index];
			index++;
		}
		if(rData[index]== '>') break;
		index++;
	}

	if((strncmp((const char*)rWord[1],Searching,6))==0){
		HAL_Delay(5000);
		ReadData(BT,rData,30,100);
		return 0;
	}

	if((strncmp((const char*)rWord[0],PID,4) == 0) && word>=5){
		rpmH = ASCII2HEX((uint8_t*)rWord[3]);
		rpmL = ASCII2HEX((uint8_t*)rWord[4]);
		return (256*rpmH+rpmL)/4;
	}

	ReadData(BT,rData,20,100);

	return 0;
//	for(int byteN=0; byteN<(respLen+2); ++byteN){
//		for(int tempDataN=0; tempDataN<=maxDatalen; ++tempDataN){
//			ReadData(BT, &tempByte, 1);
//			if(tempByte == ' ' || tempByte == '\r') break;
//			tempData[0] = tempData[1];
//			tempData[1] = tempByte;
//			if(tempDataN == (maxDatalen-1)){
//				return 0;
//			}
//		}
//		Data[byteN]= ASCII2HEX(tempData);
//	}

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
}

uint8_t Obd::IsActive(BTmodule *BT){
	char PID[] = "AT";
	uint8_t PIDLen = 2;

	char rData[10][50];
	char iByte = 0;

	int word,letter;

	SendData(BT, PID, PIDLen);
	Eol(BT);

	for(word=0; word<10; ++word){
		for(letter=0; letter<50; ++letter){
			ReadData(BT, &iByte, 1,100);
			if(iByte == ' ' || iByte == '>' || iByte == '\n' || iByte == '\0') break;
			rData[word][letter] = iByte;
		}
		if(iByte == '>') break;
	}

	for(int i=0; i<letter-1; ++i){
		if(rData[word][i] == 'A' && rData[word][i+1] == 'T') return 0;
	}
	return 1;
}

uint8_t Obd::GetSpeed(BTmodule *BT){
	char PID[] = "010D";
	uint8_t PIDLen = 4;

	uint8_t speed;

	SendData(BT, PID, PIDLen);
	Eol(BT);
	//ReadData(BT, &speed, 1);

	return speed;
}
