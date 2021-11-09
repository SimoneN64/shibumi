#include <log.h>
#include <stdarg.h>

void log_(bool fatal, const char* fmt, ...) {
  va_list lst;
  va_start(lst, fmt);
  if(fatal) {
    logfatal(fmt, lst);
  } else {
    logdebug(fmt, lst);
  }
  va_end(lst);
}