/*
 * Obd.cpp
 *
 *  Created on: 5 gru 2020
 *      Author: mikol
 */
#include "stm32f4xx_hal.h"
#include <HC05.hpp>

class Obd{
	public:
		uint16_t GetRPM(HC05 BT);
		uint8_t GetSpeed(HC05 BT);
		int GetEngineCollantTemp();
		int GetIntakeTemp();
		int GetAmbientTemp();
		int GetEngineOilTemp();
	private:
		void SendData(HC05 BT,uint8_t* data, uint8_t n);
		void ReadData(HC05 BT,uint8_t* data, uint8_t n);
		void Eol(HC05 BT);
};

void Obd::SendData(HC05 BT,uint8_t* data, uint8_t n){
	BT.DataSend(data, n);
}

void Obd::ReadData(HC05 BT,uint8_t* data, uint8_t n){
	BT.DataReceive(data, n);
}

void Obd::Eol(HC05 BT){
	uint8_t CR = '\r';
	uint8_t LF = '\n';

	BT.DataSend(&CR, 1);
	BT.DataSend(&LF, 1);
}

uint16_t Obd::GetRPM(HC05 BT){
	uint8_t PID[] = "01 0C";
	uint8_t PIDLen = 5;

	uint8_t Data[20];
	uint16_t rpm;

	SendData(BT, PID, PIDLen);
	Eol(BT);
	ReadData(BT, Data, 5);

	rpm = (256*Data[0]+Data[1])/4;

	return rpm;
}

uint8_t Obd::GetSpeed(HC05 BT){
	uint8_t PID[] = "01 0D";
	uint8_t PIDLen = 5;

	uint8_t speed;

	SendData(BT, PID, PIDLen);
	Eol(BT);
	ReadData(BT, &speed, 1);

	return speed;
}
