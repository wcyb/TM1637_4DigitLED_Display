/*
 Name:		TM1637_4DigitLED_Display.h
 Created:	01/04/2019 4:45:32 PM
 Author:	Wojciech Cybowski (github.com/wcyb)
 Editor:	http://www.visualmicro.com
 License:	GPL v2
*/

#ifndef _TM1637_4DigitLED_Display_h
#define _TM1637_4DigitLED_Display_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#ifndef ENUM_CONVERSION
#define ENUM_CONVERSION 1

template<typename Enumerator>
uint8_t getEnumValue(const Enumerator& enumType)
{
	return static_cast<uint8_t>(enumType);
}

#endif // ENUM_CONVERSION

class TM1637
{
public:
	/// <summary>
	/// Constructor of TM1637 objects. By default brightness is set to max value.
	/// </summary>
	/// <param name="dIoPin">data IO pin</param>
	/// <param name="clkPin">CLK pin</param>
	/// <param name="brightness">brightness value</param>
	TM1637(const uint8_t& dIoPin, const uint8_t& clkPin, const uint8_t& brightness = 0xFF);

	TM1637(const TM1637&) = delete;
	TM1637& operator=(const TM1637&) = delete;

	/// <summary>
	/// Destructor of TM1637 object. Clears display and switches it OFF.
	/// </summary>
	~TM1637() { clear(); setDisplayState(false); }

protected:
	/// <summary>
	/// Sends command to TM1637.
	/// </summary>
	/// <param name="commandToSend">command to send</param>
	/// <param name="addStop">true (default) to add stop after command, false otherwise</param>
	/// <returns>true if TM1637 send ACK, false otherwise</returns>
	bool sendCommand(const uint8_t& commandToSend, const bool& addStop = true);

	/// <summary>
	/// Sends data to TM1637.
	/// </summary>
	/// <param name="dataToSend">data to send</param>
	/// <param name="addStop">true to add stop after data packet, false (default) otherwise</param>
	/// <returns>true if TM1637 send ACK, false otherwise</returns>
	bool sendData(const uint8_t& dataToSend, const bool& addStop = false);

	/// <summary>
	/// Possible TM1637 data commands.
	/// </summary>
	enum class dataCommands : uint8_t
	{
		writeData = 0x40,
		readKeys = 0x42,
		writeDataFixedPosition = 0x44,
		testMode = 0x48
	};

	/// <summary>
	/// Possible TM1637 data position commands.
	/// </summary>
	enum class positionCommands : uint8_t { d1 = 0xC0, d2, d3, d4, d5, d6 };

	/// <summary>
	/// Possible TM1637 control commands.
	/// </summary>
	enum class controlCommands : uint8_t { displayOff = 0x80, displayOn = 0x88 };

public:
	/// <summary>
	/// Possible digits positions, counting from left to right.
	/// </summary>
	enum class digits : uint8_t { d1, d2, d3, d4 };

	/// <summary>
	/// Possible codings of functions arguments.
	/// </summary>
	enum class coding : uint8_t { bcd, raw };

	/// <summary>
	/// Sets hours.
	/// </summary>
	/// <param name="value">hours value</param>
	/// <param name="addColon">true to enable colon, false otherwise</param>
	/// <param name="prependZero">true to add zero before value if it is less than 10 (only in BCD mode), false otherwise</param>
	void setHours(const uint16_t& value, const bool& addColon = true, const bool& prependZero = true);

	/// <summary>
	/// Sets minutes.
	/// </summary>
	/// <param name="value">minutes value</param>
	/// <param name="prependZero">true to add zero before value if it is less than 10 (only in BCD mode), false otherwise</param>
	void setMinutes(const uint16_t& value, const bool& prependZero = true);

	/// <summary>
	/// Displays value (from left to right) on 4 digits according to coding.
	/// When value is negative, minus sign is added in front of value, if there is space left.
	/// </summary>
	/// <param name="value">value to display</param>
	/// <param name="addColon">true to enable colon, false otherwise</param>
	void setDisplayedValue(const int32_t& value, const bool& addColon = false);

	/// <summary>
	/// Sets all segments to ON.
	/// </summary>
	void testDisplay(void);

	/// <summary>
	/// Blanks given digit.
	/// </summary>
	/// <param name="digitToBlank">digit to blank</param>
	void setBlank(const digits& digitToBlank);

	/// <summary>
	/// Clears display by blanking all digits.
	/// </summary>
	void clear(void);

	/// <summary>
	/// Sets brightness according to given value. Min = 0, Max = 0xFF. Value will be converted to 8 steps supported by controller.
	/// Brightness can be changed only if display is switched ON.
	/// </summary>
	/// <param name="value">brightness value</param>
	void setBrightness(const uint8_t& value);

	/// <summary>
	/// Switches display ON or OFF.
	/// </summary>
	/// <param name="stateToSet">true to switch ON, false to switch OFF</param>
	void setDisplayState(const bool& stateToSet);

	/// <summary>
	/// Sets how values with data to display will be interpreted.
	/// </summary>
	/// <param name="codingType">coding of data to display</param>
	void setDataCoding(const coding& codingType) { m_dataCoding = codingType; }

private:
	/// <summary>
	/// Decodes digit from value at given position according to setted coding.
	/// </summary>
	/// <param name="valueToDecode">value with digit to decode</param>
	/// <param name="digit">digit from value to decode. Min = 0, Max = 3, counting from rigt to left.</param>
	/// <returns>decoded value</returns>
	uint8_t decodeValue(const uint32_t& valueToDecode, const uint8_t& digit);

	/// <summary>
	/// Sends commands to enable writing on given display position.
	/// </summary>
	/// <param name="beginFrom">beginning display position</param>
	/// <returns>true if write was confirmed by TM1637, false otherwise</returns>
	bool beginWrite(const positionCommands& beginFrom = positionCommands::d1);

	/// <summary>
	/// Returns information whether ACK was received. Used only in data transmission functions.
	/// </summary>
	/// <returns>true if ACK was received, false otherwise</returns>
	bool getAck(void);

	/// <summary>
	/// Sends START data. Used only in data transmission functions.
	/// </summary>
	void sendStart(void);

	/// <summary>
	/// Sends STOP data. Used only in data transmission functions.
	/// </summary>
	void sendStop(void);

private:
	/// <summary>
	/// Data IO pin.
	/// </summary>
	const uint8_t m_ioPin;

	/// <summary>
	/// CLK pin.
	/// </summary>
	const uint8_t m_clkPin;

	/// <summary>
	/// Selected data coding.
	/// </summary>
	coding m_dataCoding = coding::bcd;

	/// <summary>
	/// State of display. True if switched ON, false otherwise.
	/// </summary>
	bool m_displayState = false;

	/// <summary>
	/// Data used to display digits from 0 to 9.
	/// </summary>
	const uint8_t m_digitsFont[10] = { 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F };
};

#endif

