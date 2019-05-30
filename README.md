# CharLCD


C++ driver for Adafruit 16x2 character LCD shield (based on HD44780 LCD unit connected to MCP23017 I2C interface) using the Linux i2c-dev library - this code is mainly a port of the Adafruit Arduino code:

https://github.com/adafruit/Adafruit-RGB-LCD-Shield-Library

Built for Jetson Xavier (though with minor changes, it should be possible to adapt it to other Linux systems which use the i2c-dev library)

This driver can be used with either the Arduino or Raspberry Pi version, and with either the monochrome or RGB LCD unit.


## Prerequisites

i2c-dev I2C library

MCP23017 driver (included in /src directory, see also https://github.com/dehavenm/MCP23017)


## Installing

Copy contents of /src directory into project folder. See makefile in /example directory for compiling.

Wire the +5v, ground, I2C data, and I2C clock pins of the LCD shield to your device (see pinout for Arduino or Rasberry Pi) - ignore all other pins, as only those four are actually used.


## Using the library


`void CharLCD::CharLCD(int bus, int address)`

Creates an object representing an LCD shield, using the specified I2C bus and address. On the Jetson Xavier, the options for I2C bus are 1 and 8. The default address of the MCP23017 chip is 0x20.



`void CharLCD::start(uint8_t cols, uint8_t lines)`

Initializes the LCD with a specified number of columns and lines (16 and 2 for the most common Adafruid LCD shield), and starts I2C communication.


`void CharLCD::clearDisplay()`

Clears the display and sets the cursor position to (0,0).


`void CharLCD::home()`

Sets the cursor position to (0,0).


`void CharLCD::setCursor(uint8_t col, uint8_t row)`

Sets the cursor position to a specified column and line (zero index).


`void CharLCD::display()`

`void CharLCD::noDisplay()`

Turns the display on or off.


`void CharLCD::cursor()`

`void CharLCD::noCursor()`

Turns the underline cursor on or off.

`void CharLCD::blink()`

`void CharLCD::noBlink()`

Turns the blinking cursor on or off.


`void CharLCD::setBacklight(uint8_t status)`


Sets the backlight color (enter 0x00 to turn the backlight off). When using the RGB LCD, possible values are RED, YELLOW, GREEN, TEAL, BLUE, VIOLET, or WHITE. For a monochrome LCD, enter WHITE to turn the backlight on.


`void CharLCD::print(std::string text)`

Writes text to the LCD screen. The LCD text input is based on ASCII, so the string used must be ASCII formatted. 


`uint8_t CharLCD::readButtons(void)`

Returns the status of all five buttons contained in an 8-bit integer value, with each the five least significant bits representing one button (1 for pressed, 0 for not pressed). See example code lcd.cpp in /example folder for commands to get the status of each individual button.
