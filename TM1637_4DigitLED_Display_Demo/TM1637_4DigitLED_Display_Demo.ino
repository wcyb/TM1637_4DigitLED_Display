/*
 Name:		TM1637_4DigitLED_Display_Demo.ino
 Created:	01/04/2019 4:45:54 PM
 Author:	Wojciech Cybowski (github.com/wcyb)
 License:	GPL v2
*/

#include "TM1637_4DigitLED_Display.h"

TM1637* tm1637 = nullptr;

uint8_t hours = 0;
uint8_t minutes = 0;
bool colonState = false;
uint8_t brightness = 0xFF;

// the setup function runs once when you press reset or power the board
void setup() {
	tm1637 = new TM1637(52, 50);

	tm1637->testDisplay();//switch ON all segments
	delay(2500);
	tm1637->clear();//clear display
}

// the loop function runs over and over again until power down or reset
void loop() {
	if (!(millis() % 1000))//do this every second
	{
		(minutes < 59) ? minutes++ : (minutes = 0, (hours < 23) ? hours++ : hours = 0);//simulate clock
		tm1637->setHours(hours, colonState);//display hour value
		tm1637->setMinutes(minutes);//display minutes value

		colonState = colonState ? false : true;//disable or enable colon every second

		(brightness > 0) ? brightness -= 0x05 : brightness = 0xFF;//set new brightness value
		tm1637->setBrightness(brightness);//set new brightness value
	}
}
