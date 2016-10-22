#ifndef FATSTRUCT
#define FATSTRUCT

typedef struct {
  unsigned char first_byte;
  unsigned char start_chs[3];
  unsigned char partition_type;
  unsigned char end_chs[3];
  unsigned long start_sector;
  unsigned long length_sectors;
} PartitionTable;

typedef struct {
    unsigned char filename[8];
    unsigned char ext[3];
    unsigned char attributes;
    unsigned char reserved[10];
    unsigned short modify_time;
    unsigned short modify_date;
    unsigned short starting_cluster;
    unsigned long file_size;
} FileEntry;

typedef struct {
  unsigned long fat_start;
  unsigned long data_start;
  unsigned char sectors_per_cluster;
  unsigned short cluster;
  unsigned long cluster_left;
  unsigned long file_left;
} FatState;

typedef struct {
  unsigned short sector_size;
  unsigned char sectors_per_cluster;
  unsigned short reserved_sectors;
  unsigned char number_of_fats;
  unsigned short root_dir_entries;
  unsigned short total_sectors_short; // if zero, later field is used
  unsigned char media_descriptor;
  unsigned short fat_size_sectors;
} FatBootSector;

unsigned char f32_buffer[64];
FatState fat_state;

#define F32_PART ((PartitionTable *)((void *)f32_buffer))
#define F32_ENTRY ((FileEntry *)((void *)f32_buffer))
#define F32_BOOT ((FatBootSector *)((void *)f32_buffer))

#define FAT_BOOT_OFFSET 11

#endif
