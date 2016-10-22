#define PIN_SSCE   10

#include "sd.h"
#include "fat.h"

extern unsigned long sd_sector;
extern unsigned long sd_position;

#include <SPI.h>

char* SD_init() {
  char i;

  CS_DISABLE();
  SPI.begin();
  for (i = 0; i < 10; i++) {
    SPI.transfer(0xFF);
  }
  SPI.end();

  for (i = 0; i < 10 && !SD_command(0x40, 0x00000000, 0x95, 8, 0x01); i++) {
    delay(100);
  }
                        
  if (i == 10) {
    return "0x40 fail";
  }

  // CMD8
  if (!SD_command(0x48, 0x000001AA, 0x87, 8, 0x01)) {
    return "0x48 fail";
  }

  // CMD55
  if (!SD_command(0x77, 0x00000000, 0xFF, 8, 0x01)) {
    return "0x77 fail";
  }

  // ACMD41
  for (i = 0; i < 10 && !SD_command(0x69, 0x40000000, 0xFF, 8, 0x00); i++) {
    delay(400);
    SD_command(0x77, 0x00000000, 0xFF, 8, 0x01);
    delay(400);
  }

  if (i == 10) {
    return "0x69 fail";
  }

  if (!SD_command(0x50, 0x00000200, 0xFF, 8, 0x00)) {
    return "SET_BLOCKLEN failed";
  }

  return "success";
}

boolean SD_command(unsigned char cmd, unsigned long arg, unsigned char crc, unsigned char read, unsigned char expected) {
  unsigned char i, buffer[8];
  boolean contains_expected = false;

  CS_ENABLE();
  SPI.begin();
  
  SPI.transfer(cmd);
  SPI.transfer(arg >> 24);
  SPI.transfer(arg >> 16);
  SPI.transfer(arg >> 8);
  SPI.transfer(arg);
  SPI.transfer(crc);

  for (i = 0; i < read; i++) {
    buffer[i] = SPI.transfer(0xFF);
    if (buffer[i] == expected) {
      contains_expected = true;
    }
  }
  
  SPI.end();
  CS_DISABLE();

  return contains_expected;
}

void SD_read(unsigned short len) {
  unsigned short i;

  CS_ENABLE();

  SPI.begin();
  SPI.transfer(0x51);
  SPI.transfer( sd_sector >> 24);
  SPI.transfer( sd_sector >> 16);
  SPI.transfer( sd_sector >> 8);
  SPI.transfer( sd_sector );
  SPI.transfer(0xFF);

  // waiting for 0
  for (i = 0; i < 100 && SPI.transfer(0xFF) != 0x00; i++) { }

  if (i == 100) {
    SPI.end();
    return "fail, 0x00 not reached";
  }
  
  for (i = 0; i < 100 && SPI.transfer(0xFF) != 0xFE; i++) { }

  if (i == 100) {
    SPI.end();
    return "fail, 0xFE not reached";
  }

  // skip offset
  for (i = 0; i < sd_position; i++) {
    SPI.transfer(0xFF);
  }

  // read len bytes
  for (i = 0; i < len; i++) {
    f32_buffer[i] = SPI.transfer(0xFF);
  }

  // skip the rest
  for (i += sd_position; i < 512; i++) {
    SPI.transfer(0xFF);
  }

  // skip checksum
  SPI.transfer(0xFF);
  SPI.transfer(0xFF);
  SPI.end();

  CS_DISABLE();
}

void CS_DISABLE() {
  digitalWrite(PIN_SSCE, HIGH);
}

void CS_ENABLE() {
  digitalWrite(PIN_SSCE, LOW);
}

