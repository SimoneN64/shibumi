#include <pif.h>
#include <mem.h>
#include <log.h>
#include <utils.h>

#define CMD (&cmd[3])

INLINE void log_pif(mem_t* mem) {
  logdebug("PIF RAM:\n");
  for(int i = 0; i < PIF_RAM_DSIZE; i+=8) {
    logdebug("%02X %02X %02X %02X %02X %02X %02X %02X\n",
             mem->pif_ram[i], mem->pif_ram[i + 1], mem->pif_ram[i + 2], mem->pif_ram[i + 3],
             mem->pif_ram[i + 4], mem->pif_ram[i + 5], mem->pif_ram[i + 6], mem->pif_ram[i + 7]);
  }
}

static int channel = 0;

void process_pif_commands(mem_t* mem) {
  log_pif(mem);
  u8 control = mem->pif_ram[63];

  if(control & 1) {
    channel = 0;
    for(int i = 0; i < 63;) {
      u8* cmd = &mem->pif_ram[i++];
      u8 t = cmd[0] & 0x3f;

      if(t == 0 || t == 0x3D) {
        channel++;
      } else if (t == 0x3E) {
        break;
      } else if (t == 0x3F) {
        continue;
      } else {
        u8 r = mem->pif_ram[i++];
        r |= (1 << 7);
        if(r == 0xFE) {
          break;
        }

        u8 rlen = r & 0x3F;
        u8* res = &mem->pif_ram[i + t];
        switch(cmd[2]) {
          case 0:
            res[0] = 0x05;
            res[1] = 0x00;
            res[2] = 0x01;
            break;
          case 1:
            res[0] = mem->mmio.si.controller.b1;
            res[1] = mem->mmio.si.controller.b2;
            res[2] = mem->mmio.si.controller.b3;
            res[3] = mem->mmio.si.controller.b4;
            break;
          case 2: case 3: break;
          default: logfatal("Unimplemented PIF command %d\n", cmd[2]);
        }

        i += t + rlen;
        channel++;
      }

      log_pif(mem);
    }
  }

  if(control & 8) {
    mem->pif_ram[63] &= ~8;
  }

  if(control & 48) {
    mem->pif_ram[63] = 128;
  }

  //mem->pif_ram[63] &= ~1;
}