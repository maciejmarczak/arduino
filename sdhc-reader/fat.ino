#include "sd.h"
#include "fat.h"

unsigned long sd_sector;
unsigned long sd_position;

char* f32_init() {
  char i;

  f32_seek(0x1BE);

  for (i = 0; i < 4; i++) {
    f32_read(sizeof(PartitionTable));

    if (is_type_ok()) {
      break;
    }
  }

  if (i == 4) {
    return "fail, i = 4";
  }

  fat_state.fat_start = 512 * F32_PART->start_sector + FAT_BOOT_OFFSET;

  f32_seek(fat_state.fat_start);
  f32_read(sizeof(FatBootSector));

  if (F32_BOOT->sector_size == 512) {
    return "512";
  }

  return "success";
}

void f32_seek(unsigned long offset) {
  sd_sector = offset / 512; // divide by 512
  sd_position = offset % 512; // cut to 512 bytes only
}

char f32_read(unsigned char bytes) {
  SD_read(bytes);

  sd_position += (unsigned long)bytes;

  if (sd_position == 512) {
    sd_position = 0;
    sd_sector += 1;
  }

  return bytes;
}

boolean is_type_ok() {
  int type = F32_PART->partition_type;
  boolean result;

  switch (type) {
    case 4:
    case 6:
    case 11:
    case 12:
    case 14:
      result = true; break;
    default:
      result = false; break;
  }

  return result;
}

