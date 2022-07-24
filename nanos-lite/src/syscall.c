#include <common.h>
#include "syscall.h"

int sys_yield(Context *c){
	yield();
    c->GPR1	= 0;
	c->GPRx = 0;
	return 0;
}

int sys_write(int fd, void *buf, size_t count, Context *c){
	if(fd == 1 || fd == 2){
		for(int i = 0; i < count; i++){
			putch(*(char*)buf);
			(char *)buf++;
		}
		c->GPRx = count;
		return count;
	}
	c->GPRx = -1;
	return -1;
}

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  switch (a[0]) {
	  case 0: printf("Get an SYS_exit!\n");
		  halt(c->GPRx); break;
	  case 1: printf("Get an SYS_yield!\n");
			  sys_yield(c);  break;
//	  case 5: printf("Get an SYS_write!\n");
//			  sys_write(a[1], (void *)a[2], a[3], c);
//			  break;
      default: panic("Unhandled syscall ID = %d", a[0]);
  }
//  printf("Finished do_syscall!\n");
}
