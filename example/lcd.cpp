#include <CharLCD.h>

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include <cstddef>
#include <cstdint>


int main() {

  CharLCD lcd(8, 0x20);      // use default address 0x20

  lcd.start(16, 2);

  lcd.display();

  lcd.home();

  lcd.setBacklight(WHITE);

  lcd.cursor();

  lcd.blink();


  lcd.print("Hello, world!\nJetson Xavier");

  uint8_t button_data;

  while (true) //press Ctrl-C to exit
  {
    //read button data
    button_data = lcd.readButtons();

    if (button_data) {
    lcd.clearDisplay();
    lcd.setCursor(0,0);
    if (button_data & BUTTON_UP) {
      lcd.print("UP ");
      lcd.setBacklight(RED);
    }
    if (button_data & BUTTON_DOWN) {
      lcd.print("DOWN ");
      lcd.setBacklight(YELLOW);
    }
    if (button_data & BUTTON_LEFT) {
      lcd.print("LEFT ");
      lcd.setBacklight(GREEN);
    }
    if (button_data & BUTTON_RIGHT) {
      lcd.print("RIGHT ");
      lcd.setBacklight(TEAL);
    }
    if (button_data & BUTTON_SELECT) {
      lcd.print("SELECT ");
      lcd.setBacklight(VIOLET);
    }
    }

  }



  return 0;
}
