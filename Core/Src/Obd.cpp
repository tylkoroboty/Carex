///*
// * Obd.cpp
// *
// *  Created on: 5 gru 2020
// *      Author: mikol
// */
//#include "stm32f4xx_hal.h"
////#include <HC05.hpp>
//
//class Obd{
//	public:
//		uint16_t GetRPM(HC05 BT);
//		uint8_t GetSpeed(HC05 BT);
//		int GetEngineCollantTemp();
//		int GetIntakeTemp();
//		int GetAmbientTemp();
//		int GetEngineOilTemp();
//		uint8_t ASCII2HEX(uint8_t *tab);
//
//	private:
//		void SendData(HC05 BT,uint8_t* data, uint8_t n);
//		void ReadData(HC05 BT,uint8_t* data, uint8_t n);
//		void Eol(HC05 BT);
//};
//
//uint8_t Obd::ASCII2HEX(uint8_t *tab){
//
//	uint8_t A2H;
//	uint8_t Hex=0;
//
//	for(uint8_t i=0; i<2; i++){
//		if(tab[i]>=0x30 && tab[i]<=0x39){
//			//Cyfra z zakresu 0-9
//			A2H=tab[i]-0x30;
//		}
//		else if(tab[i]>=0x41 && tab[i]<=0x46){
//			//Cyfry z zakresu A-F
//			A2H=tab[i]-0x37;
//		}
//
//	Hex+=(A2H << (4*(1-i)));
//	}
//
//	return Hex;
//}
//
//void Obd::SendData(HC05 BT,uint8_t* data, uint8_t n){
//	BT.DataSend(data, n);
//}
//
//void Obd::ReadData(HC05 BT,uint8_t* data, uint8_t n){
//	BT.DataReceive(data, n);
//}
//
//void Obd::Eol(HC05 BT){
//	uint8_t CR = '\r';
//	uint8_t LF = '\n';
//
//	BT.DataSend(&CR, 1);
//	BT.DataSend(&LF, 1);
//}
//
//uint16_t Obd::GetRPM(HC05 BT){
//	uint8_t PID[] = "010C";
//	uint8_t PIDLen = 4;
//
//	uint8_t Data[20];
//	uint8_t byteReadN=10;
//
//	uint16_t rpm;
//
//	SendData(BT, PID, PIDLen);
//	Eol(BT);
//
//	for(uint8_t byteN=0; byteN<byteReadN; ++byteN){
//		//Odbieranie bajtów z ELM
//		uint8_t tempData[2];
//		uint8_t tempByte;
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
//
//	rpm = (256*Data[2]+Data[3])/4;
//
//	return rpm;
//}
//
//uint8_t Obd::GetSpeed(HC05 BT){
//	uint8_t PID[] = "010D";
//	uint8_t PIDLen = 5;
//
//	uint8_t speed;
//
//	SendData(BT, PID, PIDLen);
//	Eol(BT);
//	ReadData(BT, &speed, 1);
//
//	return speed;
//}
