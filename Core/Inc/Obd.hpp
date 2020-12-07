/*
 * Obd.hpp
 *
 *  Created on: 5 gru 2020
 *      Author: mikol
 */
#include "stm32f4xx_hal.h"
#include <HC05.hpp>

#ifndef INC_OBD_HPP_
#define INC_OBD_HPP_


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

#endif /* INC_OBD_HPP_ */
