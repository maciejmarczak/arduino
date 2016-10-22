#include "lcd.h"
#include "fat.h"

void setup(void) {
  LcdInitialise();
  LcdClear();
  SD_init();
  LcdString(f32_init());
}

void loop(void) {
  
}

