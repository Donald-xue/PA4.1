#include <common.h>
#include "syscall.h"

extern int _yield();

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  switch (a[0]) {
	  case 0: break;
	  case 1: yield(); break;
      default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
