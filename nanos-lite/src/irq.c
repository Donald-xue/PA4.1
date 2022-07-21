#include <common.h>

static Context* do_event(Event e, Context* c) {
  switch (e.event) {
	  case EVENT_YIELD: printf("Get an EVENT_YIELD!\n"); break;
	  case EVENT_SYSCALL: 
						switch (c->gpr[17]){
							case 0:
							case 1:
							case 2:
							case 3:
							case 4:
							case 5:
							case 6:
							case 7:
							case 8:
							case 9:
							case 10:
							case 11:
							case 12:
							case 13:
							case 14:
							case 15:
							case 16:
							case 17:
							case 18:
							case 19:
							   printf("Get an EVENT_SYSCALL!\n");	
						}
						break;
	  default: panic("Unhandled event ID = %d", e.event);
  }

  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
