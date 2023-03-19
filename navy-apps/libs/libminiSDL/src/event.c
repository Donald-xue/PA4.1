#include <NDL.h>
#include <SDL.h>
#include "assert.h"

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

int SDL_PushEvent(SDL_Event *ev) {
  printf("SDL_PushEvent not yet implemented");
  assert(0);
  return 0;
}

int SDL_PollEvent(SDL_Event *event) {
  char buf[64];
  int len = -1;
  if( NDL_PollEvent_refresh(buf, sizeof(buf)) > 0) {
      //printf("SDL_PollEvent key = %s, len = %d\n", buf, len);
      if(buf[1] == 'u') {
        event->key.type = SDL_KEYUP;
      } else if (buf[1] == 'd'){
        event->key.type = SDL_KEYDOWN;
      }
      switch(buf[3]) {
        case '0': event->key.keysym.sym = SDLK_0; break;
        case '1': event->key.keysym.sym = SDLK_1; break;
        case '2': event->key.keysym.sym = SDLK_2; break;
        case '3': event->key.keysym.sym = SDLK_3; break;
        case '4': event->key.keysym.sym = SDLK_4; break;
        case '5': event->key.keysym.sym = SDLK_5; break;
        case '6': event->key.keysym.sym = SDLK_6; break;
        case '7': event->key.keysym.sym = SDLK_7; break;
        case '8': event->key.keysym.sym = SDLK_8; break;
        case '9': event->key.keysym.sym = SDLK_9; break;
        case 'A':
          if(buf[4] == 'P') event->key.keysym.sym = SDLK_APOSTROPHE;
          else event->key.keysym.sym = SDLK_A; break;
        case 'B': 
          if(buf[4] == 'A' && buf[8] == 'P') event->key.keysym.sym = SDLK_BACKSPACE;
          else if(buf[4] == 'A' && buf[8] == 'L') event->key.keysym.sym = SDLK_BACKSLASH;
          else event->key.keysym.sym = SDLK_B; break;
        case 'C': 
          if(buf[4] == 'O') event->key.keysym.sym = SDLK_COMMA;
          else event->key.keysym.sym = SDLK_C; break;
        case 'D': 
          if(buf[4] == 'O') event->key.keysym.sym = SDLK_DOWN; 
          else event->key.keysym.sym = SDLK_D; break;
        case 'E':
          if(buf[4] == 'Q') event->key.keysym.sym = SDLK_EQUALS;
          else if(buf[4] == 'S') event->key.keysym.sym = SDLK_ESCAPE;
          else event->key.keysym.sym = SDLK_E; break;
        case 'F': event->key.keysym.sym = SDLK_F; break;
        case 'G': 
          if(buf[4] == 'R') event->key.keysym.sym = SDLK_GRAVE;
          else event->key.keysym.sym = SDLK_G; break;
        case 'H': event->key.keysym.sym = SDLK_H; break;
        case 'I': event->key.keysym.sym = SDLK_I; break;
        case 'J': event->key.keysym.sym = SDLK_J; break;
        case 'K': event->key.keysym.sym = SDLK_K; break;
        case 'L': 
          if(buf[4] == 'E' && buf[7] != 'B') event->key.keysym.sym = SDLK_LEFT;
          else if(buf[4] == 'E' && buf[7] == 'B') event->key.keysym.sym = SDLK_LEFTBRACKET;
          else if(buf[4] == 'C') event->key.keysym.sym = SDLK_LCTRL;
          else if(buf[4] == 'S') event->key.keysym.sym = SDLK_LSHIFT; 
          else event->key.keysym.sym = SDLK_L; break;
        case 'M':
          if(buf[4] == 'I') event->key.keysym.sym = SDLK_MINUS;
          else event->key.keysym.sym = SDLK_M; break;
        case 'N': event->key.keysym.sym = SDLK_N; break;
        case 'O': event->key.keysym.sym = SDLK_O; break;
        case 'P': 
          if(buf[4] == 'E') event->key.keysym.sym = SDLK_PERIOD;
          else event->key.keysym.sym = SDLK_P; break;
        case 'Q': event->key.keysym.sym = SDLK_Q; break;
        case 'R': 
          if(buf[4] == 'I' && buf[8] != 'B') event->key.keysym.sym = SDLK_RIGHT;
          else if(buf[4] == 'I' && buf[8] == 'B') event->key.keysym.sym = SDLK_RIGHTBRACKET;
          else if(buf[4] == 'E') event->key.keysym.sym = SDLK_RETURN; 
          else if(buf[4] == 'C') event->key.keysym.sym = SDLK_RCTRL;
          else if(buf[4] == 'S') event->key.keysym.sym = SDLK_RSHIFT;
          else event->key.keysym.sym = SDLK_R; break;
        case 'S':
          if (buf[4] == 'L') event->key.keysym.sym = SDLK_SLASH;
          else if(buf[4] == 'E') event->key.keysym.sym = SDLK_SEMICOLON;
          else if(buf[4] == 'P') event->key.keysym.sym = SDLK_SPACE;
          else event->key.keysym.sym = SDLK_S; break;
        case 'T': event->key.keysym.sym = SDLK_T; break;
        case 'U':
          if(buf[4] == 'P') event->key.keysym.sym = SDLK_UP; 
          else event->key.keysym.sym = SDLK_U; break;
        case 'V': event->key.keysym.sym = SDLK_V; break;
        case 'W': event->key.keysym.sym = SDLK_W; break;
        case 'X': event->key.keysym.sym = SDLK_X; break;
        case 'Y': event->key.keysym.sym = SDLK_Y; break;
        case 'Z': event->key.keysym.sym = SDLK_Z; break;
        default : printf("Unknown key input!\n");
                  assert(0); break;
      }
      return 1;
    }
  return 0;
}

int SDL_WaitEvent(SDL_Event *event) {
  char buf[64];
  int len = -1;
  while(1) {
    if( NDL_PollEvent(buf, sizeof(buf))) {
      printf("key = %c, len = %d\n", buf[3], len);
      if(buf[1] == 'u') {
        event->key.type = SDL_KEYUP;
      } else if (buf[1] == 'd'){
        event->key.type = SDL_KEYDOWN;
      }
      switch(buf[3]) {
        case '0': event->key.keysym.sym = SDLK_0; break;
        case '1': event->key.keysym.sym = SDLK_1; break;
        case '2': event->key.keysym.sym = SDLK_2; break;
        case '3': event->key.keysym.sym = SDLK_3; break;
        case '4': event->key.keysym.sym = SDLK_4; break;
        case '5': event->key.keysym.sym = SDLK_5; break;
        case '6': event->key.keysym.sym = SDLK_6; break;
        case '7': event->key.keysym.sym = SDLK_7; break;
        case '8': event->key.keysym.sym = SDLK_8; break;
        case '9': event->key.keysym.sym = SDLK_9; break;
        case 'J': event->key.keysym.sym = SDLK_J; break;
        case 'D': 
        if(buf[4] == 'O') event->key.keysym.sym = SDLK_DOWN; 
        else event->key.keysym.sym = SDLK_a; break;
        case 'R': 
        if(buf[4] == 'I') event->key.keysym.sym = SDLK_RIGHT; 
        else event->key.keysym.sym = SDLK_a; break;
        case 'L': 
        if(buf[4] == 'E') event->key.keysym.sym = SDLK_LEFT; 
        else event->key.keysym.sym = SDLK_a; break;
        case 'K': event->key.keysym.sym = SDLK_K; break;
        case 'U':
        if(buf[4] == 'P') event->key.keysym.sym = SDLK_UP; 
        else event->key.keysym.sym = SDLK_a; break;
        case 'G': event->key.keysym.sym = SDLK_G; break;
        default : event->key.keysym.sym = SDLK_A; break;
      }
      break;
    }
  }
  return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  printf("SDL_PeepEvents not yet implemented");
  assert(0);
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  printf("SDL_GetKeyState not yet implemented");
  assert(0);
  return NULL;
}
