/*
 * Obd.hpp
 *
 *  Created on: 5 gru 2020
 *      Author: mikol
 */
#include "stm32f4xx_hal.h"
#include "RN42.hpp"

class Obd{
public:
	uint16_t GetRPM(BTmodule *BT);
	uint8_t GetSpeed(BTmodule *BT);
	int GetEngineCollantTemp();
	int GetIntakeTemp();
	int GetAmbientTemp();
	int GetEngineOilTemp();
	uint8_t ASCII2HEX(uint8_t *tab);

private:
	void SendData(BTmodule *BT,char* data);
	void ReadData(BTmodule *BT,char* data);
	void Eol(BTmodule *BT);
};

