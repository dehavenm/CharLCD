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

  lcd.setBacklight(BLUE);

  lcd.cursor();

  lcd.blink();


  lcd.print("Hello, world!\nJetson Xavier");



  return 0;
}
