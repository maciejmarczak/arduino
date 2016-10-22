#ifndef SDCARD
#define SDCARD

char* SD_init();
boolean SD_command(unsigned char cmd, unsigned long arg, unsigned char crc, unsigned char read, unsigned char expected);
void SD_read(unsigned short len);

#endif
