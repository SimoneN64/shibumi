#include <core.h>
#include <stdio.h>
#include <log.h>

int main(int argc, const char* argv[]) {
  if(argc < 2) {
    logfatal("Please pass a valid Nintendo 64 rom as an argument.\nUsage: ./shibumi [rom]\n");
  }

  core_t core;
  init_core(&core, argv[1]);
  run_core(&core);
  
  return 0;
}