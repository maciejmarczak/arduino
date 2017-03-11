#include "sd.h"
#include "fat.h"

unsigned long sd_sector;
unsigned long sd_position;

FileEntry files[16];
char files_in_root = 0;
char pointed_file = 0;

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

  fat_state.fat_start = 512 * F32_PART->start_sector;

  f32_seek(fat_state.fat_start + FAT_BOOT_OFFSET);
  f32_read(sizeof(FatBootSector));

  if (F32_BOOT->sector_size != 512) {
    return "sector size not supported (supposed to be 512)";
  }

  // Skip reserved sectors and FAT systems
  fat_state.fat_start += F32_BOOT->reserved_sectors * 512;
  fat_state.data_start = fat_state.fat_start + (unsigned long)F32_BOOT->sectors_per_fat * (unsigned long)F32_BOOT->number_of_fats * 512;

  fat_state.sectors_per_cluster = F32_BOOT->sectors_per_cluster;

  f32_seek(fat_state.data_start);

  return "success";
}

char* f32_read_root() {
  char record_size = 32;
  boolean end_of_root = false;
  
  for (char i = 0; !end_of_root; i++) {
    f32_seek(fat_state.data_start + i * record_size);
    f32_read(sizeof(FileEntry));

    if (F32_ENTRY->filename[0] != 0) {
      if (F32_ENTRY->filename[0] == 0xE5 || !is_txt(F32_ENTRY->filename + 8, F32_ENTRY->attributes)) {
        continue;
      }
      
      for (char k = 0; k < 8; k++) {
        files[files_in_root].filename[k] = F32_ENTRY->filename[k];
      }
      
      files[files_in_root].starting_cluster = F32_ENTRY->starting_cluster;
      files[files_in_root].file_size = F32_ENTRY->file_size;
      files_in_root++;
      
    } else {
      end_of_root = true;
    }
  }

  list_files();
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

void list_files() {
  LcdInitialise();
  LcdClear();
  char page = pointed_file / 6;
  char last_id = page * 6 + 6;
  last_id = files_in_root >= last_id ? last_id : files_in_root;
  for (char i = page * 6; i < last_id; i++) {
    char left_in_line = 12;
    if (i == pointed_file) {
      LcdString(">", false, 0);
      left_in_line--;
    }
    LcdString(files[i].filename, true, left_in_line);
    delay(100);
  }
}

void print_file() {
  unsigned long first_cluster = files[pointed_file].starting_cluster;  
  unsigned long file_size = files[pointed_file].file_size;

  f32_seek(fat_state.data_start + (first_cluster - 2) * fat_state.sectors_per_cluster * 512);
  f32_read(file_size);
  f32_buffer[file_size - 1] = 0;

  LcdInitialise();
  LcdClear();
  LcdString(f32_buffer, false, 12);
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

boolean is_txt(char *ext, char attributes) {
  boolean isNormalFile = (attributes & 0b00001111) == 0;
  return ext[0] == 'T' && ext[1] == 'X' && ext[2] == 'T' && isNormalFile;
}

