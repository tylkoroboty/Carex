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

	int word,letter;
	char iByte;
	char rData[10][50];
	int rDataLen[10];

	SendData(BT, PID, PIDLen);
	Eol(BT);

	HAL_Delay(1000);

	for(word=0; word<10; ++word){
		for(letter=0; letter<50; ++letter){
			ReadData(BT, &iByte, 1);
			if(iByte == ' ' || iByte == '>' || iByte == '\r') break;
			rData[word][letter] = iByte;
		}
		rDataLen[word] = letter;
		if(iByte == '>') break;
	}


	for(int i=0; i<letter-3; ++i){
		if(rDataLen[1]>3) return 0;
		if(rData[0][i] == '0' && rData[0][i+1] == '1' && rData[0][i+2] == '0' && rData[0][i+3] == 'C'){
			uint16_t rpmH, rpmL;
			rpmH = ASCII2HEX((uint8_t*)rData[3]);
			rpmL = ASCII2HEX((uint8_t*)rData[4]);
			return (256*rpmH+rpmL)/4;
		}
	}
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
			ReadData(BT, &iByte, 1);
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
