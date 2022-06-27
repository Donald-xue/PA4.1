#include <cpu/decode.h>
#include "../local-include/rtl.h"

#define INSTR_LIST(f) f(sub) f(bgeu) f(lb) f(ori) f(slti) f(mulhu) f(sltiu) f(beq) f(bne) f(remu) f(ecall) f(csrrw) f(csrrs) f(sltu) f(xor) f(or) f(sh) f(srai) f(lbu) f(andi) f(add) f(jalr) f(auipc) f(addi) f(jal) f(lui) f(lw) f(sw) f(sll) f(and) f(xori) f(sb) f(bge) f(mul) f(div) f(lh) f(lhu) f(srli) f(slli) f(divu) f(slt) f(blt) f(rem) f(sra) f(mulh) f(bltu) f(srl) f(inv) f(nemu_trap)

def_all_EXEC_ID();
