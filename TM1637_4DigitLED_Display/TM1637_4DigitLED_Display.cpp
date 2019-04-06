/*
 Name:		TM1637_4DigitLED_Display.cpp
 Created:	01/04/2019 4:45:32 PM
 Author:	Wojciech Cybowski (github.com/wcyb)
 Editor:	http://www.visualmicro.com
 License:	GPL v2
*/

#include "TM1637_4DigitLED_Display.h"

TM1637::TM1637(const uint8_t& dIoPin, const uint8_t& clkPin, const uint8_t& brightness)
	: m_ioPin(dIoPin), m_clkPin(clkPin)
{
	digitalWrite(m_ioPin, HIGH);
	digitalWrite(m_clkPin, HIGH);//due to bug in Arduino libs, first set state of selected pin, and after this set direction, to avoid pulses
	pinMode(m_ioPin, OUTPUT);
	pinMode(m_clkPin, OUTPUT);
	clear();
	setDisplayState(true);
	setBrightness(brightness);//brightness can be changed only after switching display ON
}

void TM1637::setHours(const uint16_t& value, const bool& addColon, const bool& prependZero)
{
	uint8_t firstDigit = decodeValue(value, 1);

	if (!firstDigit && prependZero) firstDigit = decodeValue(0, 0);

	beginWrite();//start from first digit
	sendData(firstDigit);
	sendData(decodeValue(value, 0) | (addColon ? 0x80 : 0), true);
}

void TM1637::setMinutes(const uint16_t& value, const bool& prependZero)
{
	uint8_t firstDigit = decodeValue(value, 1);

	if (!firstDigit && prependZero) firstDigit = decodeValue(0, 0);

	beginWrite(positionCommands::d3);
	sendData(firstDigit);
	sendData(decodeValue(value, 0), true);
}

void TM1637::setDisplayedValue(const int32_t& value, const bool& addColon)
{
	uint32_t val = (value < 0) ? -value : value;
	uint8_t digitsSend = 0;

	if (!val)
	{
		beginWrite(positionCommands::d4);
		sendData(decodeValue(0, 0), true);
		return;
	}

	for (uint8_t i = 0; i < 4; i++, digitsSend++)
	{
		beginWrite(static_cast<positionCommands>(getEnumValue(positionCommands::d4) - i));
		if (i != 2) sendData(decodeValue(val % 10, i), true);
		else sendData(decodeValue(val % 10, i) | (addColon ? 0x80 : 0), true);
		val /= 10;
		if (!val) break;
	}

	if (value < 0 && digitsSend < 3)
	{
		beginWrite(static_cast<positionCommands>(getEnumValue(positionCommands::d4) - (digitsSend + 1)));
		sendData(0x40, true);//send minus sign
	}
}

void TM1637::testDisplay(void)
{
	beginWrite();
	for (uint8_t i = 0; i < 5; i++) sendData(0xFF);
	sendData(0xFF, true);
}

void TM1637::setBlank(const digits& digitToBlank)
{
	beginWrite(static_cast<positionCommands>(getEnumValue(positionCommands::d1) + getEnumValue(digitToBlank)));
	sendData(0, true);
}

void TM1637::clear(void)
{
	beginWrite();
	for (uint8_t i = 0; i < 5; i++) sendData(0);
	sendData(0, true);
}

void TM1637::setBrightness(const uint8_t& value)
{
	uint8_t level = m_displayState ? getEnumValue(controlCommands::displayOn) : getEnumValue(controlCommands::displayOff);//set command type, and set display ON or OFF, depending on current setting

	if (value) level |= (value / 0x20);

	sendCommand(getEnumValue(dataCommands::writeData));
	sendCommand(level);
}

void TM1637::setDisplayState(const bool& stateToSet)
{
	m_displayState = stateToSet;

	sendCommand(getEnumValue(dataCommands::writeData));
	if (stateToSet) sendCommand(getEnumValue(controlCommands::displayOn));
	else sendCommand(getEnumValue(controlCommands::displayOff));
}

bool TM1637::sendCommand(const uint8_t& commandToSend, const bool& addStop)
{
	sendStart();

	return sendData(commandToSend, addStop);
}

bool TM1637::sendData(const uint8_t& dataToSend, const bool& addStop)
{
	bool ackState;

	for (uint16_t i = 1; i <= 0x80; i <<= 1)
	{
		digitalWrite(m_clkPin, LOW);
		if (dataToSend & i) digitalWrite(m_ioPin, HIGH);
		else digitalWrite(m_ioPin, LOW);
		digitalWrite(m_clkPin, HIGH);
	}

	ackState = getAck();

	if (addStop) sendStop();

	return ackState;
}

uint8_t TM1637::decodeValue(const uint32_t& valueToDecode, const uint8_t& digit)
{
	uint8_t shift = (digit > 3) ? 3 : digit;
	uint32_t tmpVal = valueToDecode;
	uint8_t val = 0;

	if (m_dataCoding == coding::bcd)
	{
		if (!tmpVal) return m_digitsFont[0];

		for (uint8_t i = 0; i < shift + 1; i++)
		{
			if (!tmpVal) return 0;//if we want digit from position that does not exist, return 0 (blank)
			val = tmpVal % 10;
			tmpVal /= 10;
		}
		val = m_digitsFont[val];
	}
	else
	{
		if (!tmpVal) return 0;

		val = static_cast<uint8_t>((tmpVal & (0x000000FF << (8 * shift))) >> (8 * shift));
	}

	return val;
}

bool TM1637::beginWrite(const positionCommands& beginFrom)
{
	sendCommand(getEnumValue(dataCommands::writeData));
	return sendCommand(getEnumValue(beginFrom), false);//start writing from selected digit
}

bool TM1637::getAck(void)
{
	bool ackState;

	digitalWrite(m_clkPin, LOW);
	pinMode(m_ioPin, INPUT);
	digitalWrite(m_clkPin, HIGH);
	ackState = static_cast<bool>(digitalRead(m_ioPin));
	digitalWrite(m_clkPin, LOW);
	digitalWrite(m_ioPin, HIGH);//set to default state
	pinMode(m_ioPin, OUTPUT);

	return !ackState;//invert logic used by TM1637, now true means that ACK was received
}

void TM1637::sendStart(void)
{
	digitalWrite(m_ioPin, LOW);//set START by setting IO to low when CLK is high
	digitalWrite(m_clkPin, HIGH);
}

void TM1637::sendStop(void)
{
	digitalWrite(m_ioPin, LOW);
	digitalWrite(m_clkPin, HIGH);
	digitalWrite(m_ioPin, HIGH);
}
