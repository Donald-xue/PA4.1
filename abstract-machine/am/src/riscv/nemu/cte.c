#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

void __am_get_cur_as(Context *c);
void __am_switch(Context *c);

static Context* (*user_handler)(Event, Context*) = NULL;

Context* __am_irq_handle(Context *c) {
/*	for(int i = 0; i < 32; i++){
		printf("gpr(%d) = %x\n", i, c->gpr[i]);
	}
*/	
//	printf("%x %x %x\n", c->mcause, c->mstatus, c->mepc);
  __am_get_cur_as(c);
  if (user_handler) {
    Event ev = {0};
    switch (c->mcause) {
		case 0xffffffff: ev.event = EVENT_YIELD; 
						 break;
		case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7:    
		case 8: case 9: case 10: case 11: case 12: case 13: case 14: case 15: case 16: case 17: case 18: case 19:
						 ev.event = EVENT_SYSCALL; break;
		default: ev.event = EVENT_ERROR; break;
    }
    c = user_handler(ev, c);
    assert(c != NULL);
  }
//  printf("Finished __am_irq_handle!\n");
  //printf("__am_irq_handle c->pdir内容地址修改后 页表项:%p\t上下文地址%p\t所在栈帧:%p\n", c->pdir, c, &c);
  __am_switch(c);
  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));
//  printf("Initializing CTE...");

  // register event handler
  user_handler = handler;

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  Context *c = (Context *)kstack.end - sizeof(Context) - 4;
  c->mepc = (uintptr_t)entry; 
  c->gpr[10] = (uintptr_t) arg;
  c->gpr[2]  = (uintptr_t) kstack.end - 4;
  c->pdir = NULL;
//  c->mcause = -1;
  return c;
}

void yield() {
  asm volatile("li a7, -1; ecall");
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
