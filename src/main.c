#include <mem.h>
#include <stdio.h>
#include <log.h>

int main(int argc, char* argv[]) {
  if(argc < 2) {
    logfatal("Please pass a valid Nintendo 64 rom as an argument.\nUsage: ./shibumi [rom]\n");
  }
  
  return 0;
}