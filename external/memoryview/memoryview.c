#include <memoryview.h>

#define BYTES_PER_LINE 16
#define SPACE_HEX_ASCII 10

char ascii(u8 byte) {
  if (byte >= 0x20 && byte < 0x7F) {
    return (char)byte;
  } else {
    return '.';
  }
}

void igMemoryView(memory_read funcr, memory_write funcw, void* usr_data) {
  static u64 temp;
  ImVec2 glyph_size, window_size, child_size, input_size;
  igCalcTextSize(&glyph_size, "0", NULL, false, -1);
  ImGuiStyle* style = igGetStyle();
  ImVec2 constraint_min = {
    glyph_size.x * 9
      + glyph_size.x * 2 * BYTES_PER_LINE
      + glyph_size.x * (BYTES_PER_LINE - 1)
      + SPACE_HEX_ASCII
      + glyph_size.x * BYTES_PER_LINE
      + style->ScrollbarSize - 25,
    glyph_size.y
  };
  
  ImVec2 constraint_max = { constraint_min.x, __FLT_MAX__};
  
  igSetNextWindowSizeConstraints(constraint_min, constraint_max, NULL, NULL);
  igBegin("Memory viewer", NULL, 0);
  igGetWindowSize(&window_size);
  igInputInt("Address", &temp, 0, 0, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsNoBlank);
  
  igBeginChild_Str("addresses", (ImVec2){window_size.x, window_size.y - input_size.y - 65}, true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
  float line_height = glyph_size.y + style->ItemSpacing.y;
  float scroll = igGetScrollY();
  igGetWindowSize(&child_size);
  
  igSetScrollY_Float(scroll);
  
  u64 start_addr = scroll / line_height * BYTES_PER_LINE;
  u64 end_addr = (scroll + child_size.y - style->ItemSpacing.y) / line_height * BYTES_PER_LINE;

  for(u64 i = start_addr; i < end_addr; i += BYTES_PER_LINE) {
    igText("%08X", i);
    igSameLine(0, 10);

    u32 byte_1_4 = funcr(usr_data, i, false);
    u32 byte_5_8 = funcr(usr_data, i + 4, false);
    u32 byte_9_12 = funcr(usr_data, i + 8, false);
    u32 byte_12_15 = funcr(usr_data, i + 12, false);
    u32 bytes[4] = {byte_1_4, byte_5_8, byte_9_12, byte_12_15};

    for(int j = 0; j < 4; j++) {
      u8 byte1 = bytes[j] >> 24;
      u8 byte2 = bytes[j] >> 16;
      u8 byte3 = bytes[j] >> 8;
      u8 byte4 = bytes[j] & 0xff;

      igText("%02X %02X %02X %02X", byte1, byte2, byte3, byte4);
      igSameLine(0, glyph_size.x);
    }

    igSameLine(0, SPACE_HEX_ASCII);
    for(int j = 0; j < 4; j++) {
      u8 byte1 = bytes[j] >> 24;
      u8 byte2 = bytes[j] >> 16;
      u8 byte3 = bytes[j] >> 8;
      u8 byte4 = bytes[j] & 0xff;

      igText("%c%c%c%c", ascii(byte1), ascii(byte2), ascii(byte3), ascii(byte4));
      igSameLine(0, 0);
    }

    igNewLine();
  }
  igEndChild();
  igEnd();
}