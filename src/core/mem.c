#include <mem.h>
#include <tlb.h>
#include <log.h>
#include <string.h>
#include <stdio.h>
#include <utils/swap.h>
#include <utils/bit.h>
#include <utils/access.h>

void init_mem(mem_t* mem) {
  memory_regions_t* memory_regions = &mem->memory_regions;
  memory_regions->rdram = calloc(RDRAM_SIZE, 1);
  memory_regions->sram = calloc(SRAM_SIZE, 1);
  memset(memory_regions->dmem, 0, DMEM_SIZE);
  memset(memory_regions->imem, 0, IMEM_SIZE);
  memset(memory_regions->pif_ram, 0, PIF_RAM_SIZE);
  memset(memory_regions->pif_bootrom, 0, PIF_BOOTROM_SIZE);
  init_mmio(&mem->mmio);
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
  
  memory_regions_t* memory_regions = &mem->memory_regions;
  memory_regions->cart = calloc(rom_size, 1);
  
  if(fread(memory_regions->cart, 1, rom_file_size, fp) != rom_file_size) {
    printf("Failed to load rom\n");
    return false;
  }

  fclose(fp);
  u32 endianness = *(u32*)&memory_regions->cart[0];
  swap(endianness, rom_size, memory_regions->cart);
  memcpy(memory_regions->dmem, memory_regions->cart, 0x1000);
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
  memory_regions_t* memory_regions = &mem->memory_regions;
  u32 paddr = vtp(vaddr);
  switch(paddr) {
    case 0x00000000 ... 0x007FFFFF: return memory_regions->rdram[BYTE_ADDR(paddr)];
    case 0x04001000 ... 0x04001FFF: return memory_regions->imem[BYTE_ADDR(paddr) & IMEM_DSIZE];
    default: log_(FATAL, "Unimplemented %s[%08X] 8-bit read\n", regions_str(paddr), paddr); return 0;
  }
}

u16 read16(mem_t* mem, u32 vaddr) {
  memory_regions_t* memory_regions = &mem->memory_regions;
  u32 paddr = vtp(vaddr);
        
  switch(paddr) {
    case 0x00000000 ... 0x007FFFFF: return raccess(16, memory_regions->rdram, HALF_ADDR(paddr));
    case 0x04001000 ... 0x04001FFF: return raccess(16, memory_regions->imem, HALF_ADDR(paddr) & IMEM_DSIZE);
    default: log_(FATAL, "Unimplemented %s[%08X] 16-bit read\n", regions_str(paddr), paddr); return 0;
  }
}

u32 read32_(mem_t* mem, u32 vaddr, bool tlb) {
  memory_regions_t* memory_regions = &mem->memory_regions;
  u32 paddr = tlb ? vtp(vaddr) : vaddr;
  
  switch(paddr) {
    case 0x00000000 ... 0x007FFFFF: return raccess(32, memory_regions->rdram, paddr);
    case 0x04000000 ... 0x04000FFF: return raccess(32, memory_regions->dmem, paddr & DMEM_DSIZE);
    case 0x04001000 ... 0x04001FFF: return raccess(32, memory_regions->imem, paddr & IMEM_DSIZE);
    case 0x04300000 ... 0x043FFFFF: return mi_read(&mem->mmio.mi, paddr);
    case 0x04400000 ...	0x044FFFFF: return vi_read(&mem->mmio.vi, paddr);
    case 0x04600000 ... 0x046FFFFF: return pi_read(&mem->mmio.mi, &mem->mmio.pi, paddr);
    case 0x04700000 ... 0x047FFFFF: return ri_read(&mem->mmio.ri, paddr);
    case 0x10000000 ... 0x1FBFFFFF: return raccess(32, memory_regions->cart, paddr & mem->rom_mask);
    case 0x1FC00000 ... 0x1FC007BF: return raccess(32, memory_regions->pif_bootrom, paddr & PIF_BOOTROM_DSIZE);
    case 0x1FC007C0 ... 0x1FC007FF: return raccess(32, memory_regions->pif_ram, paddr & PIF_RAM_DSIZE);
    case 0x00800000 ... 0x03FFFFFF: case 0x04002000 ... 0x0403FFFF:
    case 0x04500000 ... 0x045FFFFF: case 0x04900000 ... 0x07FFFFFF:
    case 0x08000000 ... 0x0FFFFFFF: case 0x80000000 ... 0xFFFFFFFF:
    case 0x1FC00800 ... 0x7FFFFFFF: return 0;
    default: log_(WARNING, "Unimplemented %s[%08X] 32-bit read\n", regions_str(paddr), paddr); return 0;
  }
}

u64 read64(mem_t* mem, u32 vaddr) {
  memory_regions_t* memory_regions = &mem->memory_regions;
  u32 paddr = vtp(vaddr);
        
  switch(paddr) {
    case 0x00000000 ... 0x007FFFFF: return raccess(64, memory_regions->rdram, paddr); break;
    case 0x04001000 ... 0x04001FFF: return raccess(64, memory_regions->imem, paddr & IMEM_DSIZE); break;
    default: log_(FATAL, "Unimplemented %s[%08X] 64-bit read\n", regions_str(paddr), paddr); return 0;
  }
}

void write8(mem_t* mem, u32 vaddr, u8 val) {
  memory_regions_t* memory_regions = &mem->memory_regions;
  u32 paddr = vtp(vaddr);
  switch(paddr) {
    case 0x00000000 ... 0x007FFFFF: memory_regions->rdram[BYTE_ADDR(paddr)] = val; break;
    case 0x04001000 ... 0x04001FFF: memory_regions->imem[BYTE_ADDR(paddr) & IMEM_DSIZE] = val; break;
    default: log_(FATAL, "Unimplemented %s[%08X] 8-bit write (%02X)\n", regions_str(paddr), paddr, val);
  }
}

void write16(mem_t* mem, u32 vaddr, u16 val) {
  memory_regions_t* memory_regions = &mem->memory_regions;
  u32 paddr = vtp(vaddr);
        
  switch(paddr) {
    case 0x00000000 ... 0x007FFFFF: waccess(16, memory_regions->rdram, HALF_ADDR(paddr), val); break;
    case 0x04001000 ... 0x04001FFF: waccess(16, memory_regions->imem, HALF_ADDR(paddr) & IMEM_DSIZE, val); break;
    default: log_(FATAL, "Unimplemented %s[%08X] 16-bit write (%04X)\n", regions_str(paddr), paddr, val);
  }
}

void write32(mem_t* mem, registers_t* regs, u32 vaddr, u32 val) {
  memory_regions_t* memory_regions = &mem->memory_regions;
  u32 paddr = vtp(vaddr);
  
  switch(paddr) {
    case 0x00000000 ... 0x007FFFFF: waccess(32, memory_regions->rdram, paddr, val); break;
    case 0x04000000 ... 0x04000FFF: waccess(32, memory_regions->dmem, paddr & DMEM_DSIZE, val); break;
    case 0x04001000 ... 0x04001FFF: waccess(32, memory_regions->imem, paddr & IMEM_DSIZE, val); break;
    case 0x04300000 ... 0x043FFFFF: mi_write(&mem->mmio.mi, regs, paddr, val); break;
    case 0x04400000 ...	0x044FFFFF: vi_write(&mem->mmio.vi, paddr, val); break;
    case 0x04600000 ... 0x046FFFFF: pi_write(mem, regs, paddr, val); break;
    case 0x04700000 ... 0x047FFFFF: ri_write(&mem->mmio.ri, paddr, val); break;
    case 0x1FC007C0 ... 0x1FC007FF: waccess(32, memory_regions->pif_ram, paddr & PIF_RAM_DSIZE, val); break;
    case 0x00800000 ... 0x03FFFFFF: case 0x04002000 ... 0x0403FFFF:
    case 0x04500000 ... 0x045FFFFF: case 0x04900000 ... 0x07FFFFFF:
    case 0x08000000 ... 0x0FFFFFFF: case 0x80000000 ... 0xFFFFFFFF:
    case 0x1FC00800 ... 0x7FFFFFFF: break;
    default: log_(FATAL, "Unimplemented %s[%08X] 32-bit write (%08X)\n", regions_str(paddr), paddr, val);
  }
}

void write64(mem_t* mem, u32 vaddr, u64 val) {
  memory_regions_t* memory_regions = &mem->memory_regions;
  u32 paddr = vtp(vaddr);
  
  switch(paddr) {
    case 0x00000000 ... 0x007FFFFF: waccess(64, memory_regions->rdram, paddr, val); break;
    case 0x04001000 ... 0x04001FFF: waccess(64, memory_regions->imem, paddr & IMEM_DSIZE, val); break;
    default: log_(FATAL, "Unimplemented %s[%08X] 64-bit write (%16lX)\n", regions_str(paddr), paddr, val);
  }
}

u8 read8_ignore_tlb_and_maps(const u8* mem, size_t addr) {
  mem_t* mem_ = (mem_t*)mem;
  memory_regions_t* memory_regions = &mem_->memory_regions;
  switch(addr) {
    case 0x00000000 ... 0x007FFFFF: return memory_regions->rdram != NULL ? memory_regions->rdram[addr] : 0;
    case 0x04000000 ... 0x04000FFF: return memory_regions->dmem[addr & DMEM_DSIZE];
    case 0x04001000 ... 0x04001FFF: return memory_regions->imem[addr & IMEM_DSIZE];
    case 0x04300000 ... 0x043FFFFF: return mi_read8(&mem_->mmio.mi, addr);
    case 0x04400000 ...	0x044FFFFF: return vi_read8(&mem_->mmio.vi, addr);
    case 0x04600000 ... 0x046FFFFF: return pi_read8(&mem_->mmio.mi, &mem_->mmio.pi, addr);
    case 0x04700000 ... 0x047FFFFF: return ri_read8(&mem_->mmio.ri, addr);
    case 0x10000000 ... 0x1FBFFFFF: return memory_regions->cart != NULL ? memory_regions->cart[addr & mem_->rom_mask] : 0;
    case 0x1FC00000 ... 0x1FC007BF: return memory_regions->pif_bootrom[addr & PIF_BOOTROM_DSIZE];
    case 0x1FC007C0 ... 0x1FC007FF: return memory_regions->pif_ram[addr & PIF_RAM_DSIZE];
    case 0x00800000 ... 0x03FFFFFF: case 0x04002000 ... 0x0403FFFF:
    case 0x04500000 ... 0x045FFFFF: case 0x04900000 ... 0x07FFFFFF:
    case 0x08000000 ... 0x0FFFFFFF: case 0x80000000 ... 0xFFFFFFFF:
    case 0x1FC00800 ... 0x7FFFFFFF: return 0;
    default: log_(WARNING, "Unimplemented %s[%08zX] 32-bit read\n", regions_str(addr), addr); return 0;
  }
}