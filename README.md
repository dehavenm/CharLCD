# CharLCD


C++ driver for Adafruit 16x2 character LCD shield (based on HD44780 LCD unit connected to MCP23017 I2C interface) using the Linux i2c-dev library

Built for Jetson Xavier (though with minor changes, it should be possible to adapt it to other Linux systems which use the i2c-dev library)

This driver can be used with either the Arduino or Raspberry Pi version, and with either the monochrome or RGB LCD unit.


## Prerequisites

i2c-dev I2C library

MCP23017 driver (included in /src directory, see also https://github.com/dehavenm/MCP23017)


## Installing

Copy contents of /src directory into project folder. See makefile in /example directory for compiling.


## Using the library


`CharLCD::CharLCD(int bus, int address)`

Creates an object representing an LCD shield, using the specified I2C bus and address. On the Jetson Xavier, the options for I2C bus are 1 and 8. The default address of the MCP23017 chip is 0x20.



`CharLCD::start(uint8_t cols, uint8_t lines)`

Initializes the LCD with a specified number of columns and lines (16 and 2 for the most common Adafruid LCD shield), and starts I2C communication.


`CharLCD::clearDisplay()`

Clears the display and sets the cursor position to (0,0).


`CharLCD::home()`

Sets the cursor position to (0,0).
