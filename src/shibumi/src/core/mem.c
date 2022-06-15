#include <mem.h>
#include <tlb.h>
#include <log.h>
#include <string.h>
#include <stdio.h>
#include <utils/swap.h>
#include <utils/bit.h>
#include <utils/access.h>

void init_mem(mem_t* mem) {
  mem->rdram = calloc(RDRAM_SIZE, 1);
  mem->sram = calloc(SRAM_SIZE, 1);
  memset(mem->dmem, 0, DMEM_SIZE);
  memset(mem->imem, 0, IMEM_SIZE);
  memset(mem->pif_ram, 0, PIF_RAM_SIZE);
  memset(mem->pif_bootrom, 0, PIF_BOOTROM_SIZE);
  init_mmio(&mem->mmio);
}

void destroy_mem(mem_t* mem) {
  free(mem->rdram);
  free(mem->sram);
}

bool load_rom(mem_t* mem, const char* path) {
  FILE* fp = fopen(path, "rb");
  if(fp == NULL) {
    printf("Failed to open rom %s\n", path);
    return false;
  }

  fseek(fp, 0, SEEK_END);
  size_t rom_file_size = ftell(fp);
  size_t rom_size = next_pow_2(rom_file_size);
  mem->rom_mask = rom_size - 1; 
  fseek(fp, 0, SEEK_SET);
  
  mem->cart = calloc(rom_size, 1);
  
  if(fread(mem->cart, 1, rom_file_size, fp) != rom_file_size) {
    printf("Failed to load rom\n");
    return false;
  }

  fclose(fp);
  swap(rom_size, mem->cart);
  memcpy(mem->dmem, mem->cart, 0x1000);
  return true;
}

const char* regions_str(u32 paddr) {
  switch(paddr) {
    case 0x00000000 ... 0x007FFFFF: return "RDRAM";
    case 0x00800000 ... 0x03EFFFFF:
    case 0x04002000 ... 0x0403FFFF:
    case 0x04900000 ...	0x04FFFFFF: return "Unused";
    case 0x03F00000 ... 0x03FFFFFF: return "RDRAM Registers";
    case 0x04000000 ... 0x04000FFF: return "DMEM";
    case 0x04001000 ... 0x04001FFF: return "IMEM";
    case 0x04040000 ...	0x040FFFFF: return "SP Registers";
    case 0x04100000 ...	0x041FFFFF: return "DP Command Registers";
    case 0x04200000 ...	0x042FFFFF: return "DP Span Registers";
    case 0x04300000 ...	0x043FFFFF: return "MIPS Interface";
    case 0x04400000 ...	0x044FFFFF: return "Video Interface";
    case 0x04500000 ...	0x045FFFFF: return "Audio Interface";
    case 0x04600000 ...	0x046FFFFF: return "Peripheral Interface";
    case 0x04700000 ...	0x047FFFFF: return "RDRAM Interface";
    case 0x04800000 ...	0x048FFFFF: return "Serial Interface";
    case 0x05000000 ...	0x05FFFFFF: return "Cartridge Domain 2 Address 1";
    case 0x06000000 ...	0x07FFFFFF: return "Cartridge Domain 1 Address 1";
    case 0x08000000 ...	0x0FFFFFFF: return "Cartridge Domain 2 Address 2";
    case 0x10000000 ...	0x1FBFFFFF: return "Cartridge Domain 1 Address 2";
    case 0x1FC00000 ...	0x1FC007BF: return "PIF Bootrom";
    case 0x1FC007C0 ...	0x1FC007FF: return "PIF RAM";
    case 0x1FC00800 ...	0x1FCFFFFF: return "Reserved";
    case 0x1FD00000 ...	0x7FFFFFFF: return "Cartridge Domain 1 Address 3";
    case 0x80000000 ... 0xFFFFFFFF: return "Unknown";
    default: logfatal("Unhandled region string (%08X)\n", paddr);
  }
}

u8 read8(mem_t* mem, u32 vaddr) {
  u32 paddr = vtp(vaddr);
  switch(paddr) {
    case 0x00000000 ... 0x007FFFFF: return mem->rdram[paddr];
    case 0x04001000 ... 0x04001FFF: return mem->imem[paddr & IMEM_DSIZE];
    default: logfatal("Unimplemented %s[%08X] 8-bit read\n", regions_str(paddr), paddr);
  }
}

u16 read16(mem_t* mem, u32 vaddr) {
  u32 paddr = vtp(vaddr);
        
  switch(paddr) {
    case 0x00000000 ... 0x007FFFFF: return raccess(16, mem->rdram, paddr);
    case 0x04001000 ... 0x04001FFF: return raccess(16, mem->imem, paddr & IMEM_DSIZE);
    default: logfatal("Unimplemented %s[%08X] 16-bit read\n", regions_str(paddr), paddr);
  }
}

u32 read32(mem_t* mem, u32 vaddr, s64 pc) {
  u32 paddr = vtp(vaddr);
  
  switch(paddr) {
    case 0x00000000 ... 0x007FFFFF: return raccess(32, mem->rdram, paddr);
    case 0x04000000 ... 0x04000FFF: return raccess(32, mem->dmem, paddr & DMEM_DSIZE);
    case 0x04001000 ... 0x04001FFF: return raccess(32, mem->imem, paddr & IMEM_DSIZE);
    case 0x04040000 ... 0x040FFFFF: case 0x04300000 ...	0x044FFFFF:
    case 0x04500000 ... 0x048FFFFF: return read_mmio(&mem->mmio, paddr);
    case 0x10000000 ... 0x1FBFFFFF: return raccess(32, mem->cart, paddr & mem->rom_mask);
    case 0x1FC00000 ... 0x1FC007BF: return raccess(32, mem->pif_bootrom, paddr & PIF_BOOTROM_DSIZE);
    case 0x1FC007C0 ... 0x1FC007FF: return raccess(32, mem->pif_ram, paddr & PIF_RAM_DSIZE);
    case 0x00800000 ... 0x03FFFFFF: case 0x04002000 ... 0x0403FFFF:
    case 0x04900000 ... 0x07FFFFFF: case 0x08000000 ... 0x0FFFFFFF:
    case 0x80000000 ... 0xFFFFFFFF: case 0x1FC00800 ... 0x7FFFFFFF: return 0;
    default: logfatal("Unimplemented %s[%08X] 32-bit read (PC = %016lX)\n", regions_str(paddr), paddr, pc);
  }
}

u64 read64(mem_t* mem, u32 vaddr) {
  u32 paddr = vtp(vaddr);
        
  switch(paddr) {
    case 0x00000000 ... 0x007FFFFF: return raccess(64, mem->rdram, paddr);
    case 0x04001000 ... 0x04001FFF: return raccess(64, mem->imem, paddr & IMEM_DSIZE);
    default: logfatal("Unimplemented %s[%08X] 64-bit read\n", regions_str(paddr), paddr);
  }
}

void write8(mem_t* mem, u32 vaddr, u8 val) {
  u32 paddr = vtp(vaddr);
  switch(paddr) {
    case 0x00000000 ... 0x007FFFFF: mem->rdram[paddr] = val; break;
    case 0x04001000 ... 0x04001FFF: mem->imem[paddr & IMEM_DSIZE] = val; break;
    default: logfatal("Unimplemented %s[%08X] 8-bit write (%02X)\n", regions_str(paddr), paddr, val);
  }
}

void write16(mem_t* mem, u32 vaddr, u16 val) {
  u32 paddr = vtp(vaddr);
        
  switch(paddr) {
    case 0x00000000 ... 0x007FFFFF: waccess(16, mem->rdram, paddr, val); break;
    case 0x04001000 ... 0x04001FFF: waccess(16, mem->imem, paddr & IMEM_DSIZE, val); break;
    default: logfatal("Unimplemented %s[%08X] 16-bit write (%04X)\n", regions_str(paddr), paddr, val);
  }
}

void write32(mem_t* mem, registers_t* regs, u32 vaddr, u32 val) {
  u32 paddr = vtp(vaddr);
  
  switch(paddr) {
    case 0x00000000 ... 0x007FFFFF: waccess(32, mem->rdram, paddr, val); break;
    case 0x04000000 ... 0x04000FFF: waccess(32, mem->dmem, paddr & DMEM_DSIZE, val); break;
    case 0x04001000 ... 0x04001FFF: waccess(32, mem->imem, paddr & IMEM_DSIZE, val); break;
    case 0x04040000 ... 0x040FFFFF: case 0x04300000 ...	0x044FFFFF:
    case 0x04500000 ... 0x048FFFFF: write_mmio(mem, regs, &mem->mmio.si, paddr, val); break;
    case 0x1FC007C0 ... 0x1FC007FF: waccess(32, mem->pif_ram, paddr & PIF_RAM_DSIZE, val); break;
    case 0x00800000 ... 0x03FFFFFF: case 0x04002000 ... 0x0403FFFF:
    case 0x04900000 ... 0x07FFFFFF: case 0x08000000 ... 0x0FFFFFFF:
    case 0x80000000 ... 0xFFFFFFFF: case 0x1FC00800 ... 0x7FFFFFFF: break;
    default: logfatal("Unimplemented %s[%08X] 32-bit write (%08X)\n", regions_str(paddr), paddr, val);
  }
}

void write64(mem_t* mem, u32 vaddr, u64 val) {
  u32 paddr = vtp(vaddr);
  
  switch(paddr) {
    case 0x00000000 ... 0x007FFFFF: waccess(64, mem->rdram, paddr, val); break;
    case 0x04001000 ... 0x04001FFF: waccess(64, mem->imem, paddr & IMEM_DSIZE, val); break;
    default: logfatal("Unimplemented %s[%08X] 64-bit write (%16lX)\n", regions_str(paddr), paddr, val);
  }
}
