#include <CharLCD.h>
#include <stdio.h>
#include <inttypes.h>
#include <unistd.h>

#include <cstddef>
#include <cstdint>
#include <string>


// When the display powers up, it is configured as follows:
//
// 1. Display clear
// 2. Function set:
//    DL = 1; 8-bit interface data
//    N = 0; 1-line display
//    F = 0; 5x8 dot character font
// 3. Display on/off control:
//    D = 0; Display off
//    C = 0; Cursor off
//    B = 0; Blinking off
// 4. Entry mode set:
//    I/D = 1; Increment by 1
//    S = 0; No shift
//
// Note, however, that resetting the Arduino doesn't reset the LCD, so we
// can't assume that its in that state when a sketch starts (and the
// RGBLCDShield constructor is called).

CharLCD::CharLCD(int bus, int address) 
  : _i2c(bus, address)
{

  _displayfunction = LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS;

  // the I/O expander pinout
  _rs_pin = 15;
  _rw_pin = 14;
  _enable_pin = 13;
  _data_pins[0] = 12;  // really d4
  _data_pins[1] = 11;  // really d5
  _data_pins[2] = 10;  // really d6
  _data_pins[3] = 9;  // really d7

  _button_pins[0] = 0;
  _button_pins[1] = 1;
  _button_pins[2] = 2;
  _button_pins[3] = 3;
  _button_pins[4] = 4;
  // we can't begin() yet :(
}

void CharLCD::start(uint8_t cols, uint8_t lines) {

    _i2c.openI2C();

    _i2c.pinMode(8, OUTPUT);
    _i2c.pinMode(6, OUTPUT);
    _i2c.pinMode(7, OUTPUT);
    setBacklight(0x7);

    _i2c.pinMode(_rw_pin, OUTPUT);

    _i2c.pinMode(_rs_pin, OUTPUT);
    _i2c.pinMode(_enable_pin, OUTPUT);
    for (uint8_t i=0; i<4; i++)
      _i2c.pinMode(_data_pins[i], OUTPUT);

    for (uint8_t i=0; i<5; i++) {
      _i2c.pinMode(_button_pins[i], INPUT);
      _i2c.pullUp(_button_pins[i], HIGH);
    }



  _numlines = lines;
  _currline = 0;
  _numcols = cols;
  _currcol = 0;


  // SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
  // according to datasheet, we need at least 40ms after power rises above 2.7V
  // before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
  usleep(50000);
  // Now we pull both RS and R/W low to begin commands
  _i2c.digitalWrite(_rs_pin, LOW);
  _i2c.digitalWrite(_enable_pin, LOW);
  if (_rw_pin != 255) {
    _i2c.digitalWrite(_rw_pin, LOW);


  }

  //put the LCD into 4 bit or 8 bit mode
  if (! (_displayfunction & LCD_8BITMODE)) {
    // this is according to the hitachi HD44780 datasheet
    // figure 24, pg 46

    // we start in 8bit mode, try to set 4 bit mode
    write4bits(0x03);
    usleep(4500); // wait min 4.1ms

    // second try
    write4bits(0x03);
    usleep(4500); // wait min 4.1ms

    // third go!
    write4bits(0x03);
    usleep(150);

    // finally, set to 8-bit interface
    write4bits(0x02);
  } else {
    // this is according to the hitachi HD44780 datasheet
    // page 45 figure 23

    // Send function set command sequence
    command(LCD_FUNCTIONSET | _displayfunction);
    usleep(4500);  // wait more than 4.1ms

    // second try
    command(LCD_FUNCTIONSET | _displayfunction);
    usleep(150);

    // third go
    command(LCD_FUNCTIONSET | _displayfunction);
  }

  // finally, set # lines, font size, etc.
  command(LCD_FUNCTIONSET | _displayfunction);

  // turn the display on with no cursor or blinking default
  _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
  display();

  // clear it off
  clearDisplay();

  // Initialize to default text direction (for romance languages)
  _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
  // set the entry mode
  command(LCD_ENTRYMODESET | _displaymode);

  // Set text entry to left-to-right (default)
  leftToRight();

  setCursor(0, 0);

}


/********** high level commands, for the user! */
void CharLCD::clearDisplay()
{
  command(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
  usleep(2000);  // this command takes a long time!
}

void CharLCD::home()
{
  command(LCD_RETURNHOME);  // set cursor position to zero
  _currline = 0;
  _currcol = 0;
  usleep(2000);  // this command takes a long time!
}

void CharLCD::setCursor(uint8_t col, uint8_t row)
{
  int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
  if ( row > _numlines ) {
    row = _numlines-1;    // we count rows starting w/0
  }

  _currline = row;
  _currcol = col;

  command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turn the display on/off (quickly)
void CharLCD::noDisplay() {
  _displaycontrol &= ~LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void CharLCD::display() {
  _displaycontrol |= LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void CharLCD::noCursor() {
  _displaycontrol &= ~LCD_CURSORON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void CharLCD::cursor() {
  _displaycontrol |= LCD_CURSORON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void CharLCD::noBlink() {
  _displaycontrol &= ~LCD_BLINKON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void CharLCD::blink() {
  _displaycontrol |= LCD_BLINKON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void CharLCD::scrollDisplayLeft(void) {
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void CharLCD::scrollDisplayRight(void) {
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void CharLCD::leftToRight(void) {
  _displaymode |= LCD_ENTRYLEFT;
  command(LCD_ENTRYMODESET | _displaymode);
  _left_to_right = true;
}

// This is for text that flows Right to Left
void CharLCD::rightToLeft(void) {
  _displaymode &= ~LCD_ENTRYLEFT;
  command(LCD_ENTRYMODESET | _displaymode);
  _left_to_right = false;
}

// This will 'right justify' text from the cursor
void CharLCD::autoscroll(void) {
  _displaymode |= LCD_ENTRYSHIFTINCREMENT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void CharLCD::noAutoscroll(void) {
  _displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void CharLCD::createChar(uint8_t location, uint8_t charmap[]) {
  location &= 0x7; // we only have 8 locations 0-7
  command(LCD_SETCGRAMADDR | (location << 3));
  for (int i=0; i<8; i++) {
    write(charmap[i]);
  }
  command(LCD_SETDDRAMADDR);  // unfortunately resets the location to 0,0
}


// Print text to LCD screen

void CharLCD::print(std::string text) {

    int line = _currline;
    int col = _currcol;

    char current_char;

    int len = text.size();
    //iterate through each character
    for (int i=0; i<len; i++) {
        current_char = text.at(i);

        if (current_char == '\n' ){
            line++;

            if (_left_to_right) {
                col = 0;
            }
            else {
                col = _numcols - 1;
            }
            setCursor(col, line);

        }
        else if (col > (_numcols-1) || col < 0){
            line++;

            if (_left_to_right) {
                col = 0;
            }
            else {
                col = _numcols - 1;
            }
            setCursor(col, line);
            write(current_char);
            if (_left_to_right) {
                col++;
            }
            else {
                col--;
            }
        }
        else {
            write(current_char);
            if (_left_to_right) {
                col++;
            }
            else {
                col--;
            }
        }


    }


}

/*********** mid level commands, for sending data/cmds */

inline void CharLCD::command(uint8_t value) {
  send(value, LOW);
}


inline void CharLCD::write(uint8_t value) {
  send(value, HIGH);
}

/************ low level data pushing commands **********/


// Allows to set the backlight, if the LCD backpack is used
void CharLCD::setBacklight(uint8_t status) {
  // check if i2c or SPI
  _i2c.digitalWrite(8, ~(status >> 2) & 0x1);
  _i2c.digitalWrite(7, ~(status >> 1) & 0x1);
  _i2c.digitalWrite(6, ~status & 0x1);
}


// write either command or data, with automatic 4/8-bit selection
void CharLCD::send(uint8_t value, uint8_t mode) {
  _i2c.digitalWrite(_rs_pin, mode);

  // if there is a RW pin indicated, set it low to Write
  if (_rw_pin != 255) {
    _i2c.digitalWrite(_rw_pin, LOW);
  }

  if (_displayfunction & LCD_8BITMODE) {
    write8bits(value);
  } else {
    write4bits(value>>4);
    write4bits(value);
  }
}

void CharLCD::pulseEnable(void) {
  _i2c.digitalWrite(_enable_pin, LOW);
  usleep(1);
  _i2c.digitalWrite(_enable_pin, HIGH);
  usleep(1);    // enable pulse must be >450ns
  _i2c.digitalWrite(_enable_pin, LOW);
  usleep(100);   // commands need > 37us to settle
}


void CharLCD::write4bits(uint8_t value) {
    for (int i = 0; i < 4; i++) {
     _i2c.pinMode(_data_pins[i], OUTPUT);
     _i2c.digitalWrite(_data_pins[i], (value >> i) & 0x01);
    }
    pulseEnable();
}


void CharLCD::write8bits(uint8_t value) {
  for (int i = 0; i < 8; i++) {
    _i2c.pinMode(_data_pins[i], OUTPUT);
    _i2c.digitalWrite(_data_pins[i], (value >> i) & 0x01);
  }

  pulseEnable();
}

uint8_t CharLCD::readButtons(void) {
  uint8_t reply = 0x1F;

  for (uint8_t i=0; i<5; i++) {
    reply &= ~((_i2c.digitalRead(_button_pins[i])) << i);
  }
  return reply;
}
