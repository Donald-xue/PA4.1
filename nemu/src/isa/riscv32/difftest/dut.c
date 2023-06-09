#include <isa.h>
#include <cpu/difftest.h>
#include "../local-include/reg.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
	int flag = 0;
	for(int i = 0; i < 32; i++){
		if( gpr(i) != ref_r->gpr[i]._32 ){
			flag = 1;
			printf("gpr(%d) = %x, ref_r->gpr[%d] = %x!!\n ",i, gpr(i),i, ref_r->gpr[i]._32);
		}
	}
	if(ref_r->pc != cpu.pc){
		flag = 1;
		printf("!!!!!!!!!!!!!pc = %x, dut pc = %x, ref_r->pc = %x\n", pc, cpu.pc    , ref_r->pc);
	}
	if(flag == 0)
		return true;
	else
		return false;
}

void isa_difftest_attach() {
}
