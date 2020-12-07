/*
 * HC05.hpp
 *
 *  Created on: Nov 24, 2020
 *      Author: mikol
 */

#include "stm32f429xx.h"

#ifndef INC_HC05_HPP_
#define INC_HC05_HPP_

enum MODE:uint8_t {DATA, AT};
enum ROLE:uint8_t {SLAVE, MASTER};
enum STATUS:uint8_t {OK, ERR};

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
	STATUS  ATInit();
	STATUS ATBind(uint8_t *Address);
	STATUS ATLink(uint8_t *Address);
	STATUS ATCmode(uint8_t Cmode);
	STATUS ATClearPairList();

	STATUS ATReset();

	void ATSetDefault();

protected:

	void KeyEnable();
	void KeyDisable();
	void ATSendCommand(uint8_t *Command, uint8_t n);
	void ATEol();

	/* KEY Pin */
	uint16_t itskeyPin;
	GPIO_TypeDef *itskeyPort;

	/* UART */
	UART_HandleTypeDef *itsuart;

	uint8_t itsMode = AT;
};


#endif /* INC_HC05_HPP_ */
