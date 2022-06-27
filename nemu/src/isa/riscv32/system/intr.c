#include <isa.h>

//struct Context irstate = {0};

word_t isa_raise_intr(word_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * Then return the address of the interrupt/exception vector.
   */
	cpu.mepc = epc;
	cpu.mcause = NO;
	uint32_t no = cpu.mtvec;
//	printf("!!!!!!!!cpu.mtvec = %x", cpu.mtvec);
	cpu.pc = no;

  return cpu.pc;
}

word_t isa_query_intr() {
  return INTR_EMPTY;
}
