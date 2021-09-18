#include <mem.h>
#include <stdlib.h>
#include <stdio.h>

void init_mem(mem_t* mem) {
  mem->rdram = malloc(0x800000);
  mem->sram = malloc(0x8000000);
  memset(mem->rdram, 0, RDRAM_SIZE);
  memset(mem->sram, 0, SRAM_SIZE);
  memset(mem->dmem, 0, DMEM_SIZE);
  memset(mem->imem, 0, IMEM_SIZE);
  memset(mem->pif_ram, 0, PIF_RAM_SIZE);
  memset(mem->pif_bootrom, 0, PIF_BOOTROM_SIZE);
}

void load_rom(const char* path, mem_t* mem) {
  FILE* fp = fopen(path, "rb");
  if(fp == NULL) {
    logfatal("Failed to open rom %s\n", path);
  }

  fseek(fp, 0, SEEK_END);
  size_t rom_size = ftell(fp);
  mem->rom_size = rom_size; 
  fseek(fp, 0, SEEK_SET);
  mem->rom = malloc(rom_size);
  if(fread(mem->rom, 1, rom_size, fp) != rom_size) {
    logfatal("Failed to load rom\n");
  }
  fclose(fp);
}

u8 read8(mem_t* mem) {

}

u16 read16(mem_t* mem) {

}

u32 read32(mem_t* mem) {

}

void write8(mem_t* mem, u8 val) {

}

void write16(mem_t* mem, u16 val) {

}

void write32(mem_t* mem, u32 val) {

}