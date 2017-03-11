#include "lcd.h"
#include "fat.h"

int enterPin = 2;
int swapPin = 3;
bool is_file_open = false;
extern char pointed_file;
extern char files_in_root;

void setup(void) {
  pinMode(enterPin, INPUT);
  pinMode(swapPin, INPUT);
  digitalWrite(swapPin, LOW);
  digitalWrite(enterPin, LOW);
  
  SD_init();
  f32_init();
  f32_read_root();
}

void loop(void) {
  int enterPinVal = digitalRead(enterPin);
  if (enterPinVal == HIGH) {
    if (!is_file_open) {
      print_file();
    } else {
      list_files();
    }
    is_file_open = !is_file_open;
    delay(100);
  }
  int swapPinVal = digitalRead(swapPin);
  if (swapPinVal == HIGH) {
    pointed_file = ++pointed_file % files_in_root;
    list_files();
    delay(100);
  }
}

