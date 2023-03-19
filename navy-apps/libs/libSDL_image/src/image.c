#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SDL_malloc  malloc
#define SDL_free    free
#define SDL_realloc realloc

#define SDL_STBIMAGE_IMPLEMENTATION
#include "SDL_stbimage.h"

SDL_Surface* IMG_Load_RW(SDL_RWops *src, int freesrc) {
  assert(src->type == RW_TYPE_MEM);
  assert(freesrc == 0);
  return NULL;
}

SDL_Surface* IMG_Load(const char *filename) {
  FILE *fp = fopen(filename, "r+");
  int fd = open(filename, O_RDWR);
  printf("file: %s\n", filename);
  assert(fp);
  fseek(fp, 0L, SEEK_END);
  int size = ftell(fp);
  fseek(fp, 0L, SEEK_SET);
//  struct stat statbuf;
//  stat(filename, &statbuf);
//  uint32_t s = statbuf.st_size;
  
  char * buf = (char *) SDL_malloc(size+1);
//  int count = fread(buf, size, 1, fp);
  int count = read(fd, buf, size);
  SDL_Surface *surface = STBIMG_LoadFromMemory(buf, size);
  fclose(fp);
  free(buf);
//  printf("size = %x\n", size);
  return surface;
}

int IMG_isPNG(SDL_RWops *src) {
  return 0;
}

SDL_Surface* IMG_LoadJPG_RW(SDL_RWops *src) {
  return IMG_Load_RW(src, 0);
}

char *IMG_GetError() {
  return "Navy does not support IMG_GetError()";
}
