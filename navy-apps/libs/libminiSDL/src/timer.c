#include <NDL.h>
#include <sdl-timer.h>
#include <stdio.h>
#include <assert.h>

SDL_TimerID SDL_AddTimer(uint32_t interval, SDL_NewTimerCallback callback, void *param) {
//  printf("SDL_AddTimer not yet implemented");
//  assert(0);
  return NULL;
}

int SDL_RemoveTimer(SDL_TimerID id) {
//  printf("SDL_RemoveTimer not yet implemented");
//  assert(0);
  return 1;
}

uint32_t SDL_GetTicks() {
  uint32_t ticks;
  ticks = NDL_GetTicks();
  return ticks;
}

void SDL_Delay(uint32_t ms) {
  //printf("SDL_Delay not yet implemented");
  //assert(0);
}
