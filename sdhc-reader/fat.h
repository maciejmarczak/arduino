#ifndef FAT32
#define FAT32

#include "fat_structures.h"

char* f32_init();
void f32_seek(unsigned long offset);
char f32_read(unsigned char bytes);
void print_file();

#endif
