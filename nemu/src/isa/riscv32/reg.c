#include <isa.h>
#include "local-include/reg.h"

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

void isa_reg_display() {
	for( int i = 0; i < 32; i++ )
	{
		printf( "%-5s %-40u 0x %-40x\n", regs[i], cpu.gpr[i]._32, cpu.gpr[i]._32 );
	}
	printf( "pc\t %-40u 0x %-40x\n", cpu.pc, cpu.pc );
}

word_t isa_reg_str2val(const char *s, bool *success) {
	int val = 0,i = 0, pc = 0;
	if(strcmp(s, "pc") == 0){
		pc = 1;
		val = cpu.pc;
		return val;
	}
	for(; i < 32; i++){
		if(strcmp(s,regs[i]) == 0){
			val = cpu.gpr[i]._32;
			break;
		}
		if(strcmp(s, "pc") == 0){
			val = cpu.pc;
			break;
		}
	}
	if(i < 32 || pc == 1)
		*success = true;
	if(i == 32 && pc == 0){
		*success = false;
		printf("Can't find the register!\n");
//		assert(0);
	}
//	printf("%u\n", (int)success);
	return val;
}
