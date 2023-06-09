int call = 0;
extern int fnum;
extern char *sign_data;                                                  
extern char *sym_data;
extern word_t isa_raise_intr(word_t NO, vaddr_t epc);
//extern void isa_reg_display();

def_EHelper(ecall) {
//	printf("!!!!!!!!!pc = %x", cpu.pc);
//	isa_reg_display();
	isa_raise_intr(cpu.gpr[17]._32, s->pc);
	s->dnpc = cpu.pc;
//	isa_reg_display();
//	printf("!!!!!!!!!pc = %x", cpu.pc);
}

def_EHelper(mret) {
	difftest_skip_ref();
//	if(cpu.mcause == 0xffffffff){
		s->dnpc = cpu.mepc + 4;
//	}
}

def_EHelper(bne) {
    if(*dsrc1 != *ddest){
        uint32_t off = id_src2->imm;
        uint32_t p11 = off % 2;
        off = off >> 1;
        uint32_t p1 = off % 1024;
        off = off >> 10;
        uint32_t p12 = off % 2;
        off = off >> 1;
		uint32_t fin = (p12 << 11) | (p11 << 10) | p1;
		fin = fin * 2;
		int32_t dm = (int32_t) fin;
        dm <<= 32 - 13;
        dm >>= 32 - 13;
        fin = dm;
        rtl_sext(s, &id_src2->imm, &fin, 4);
		rtl_addi(s, &s->dnpc, &s->pc, fin);
	} 
}

def_EHelper(bltu) {
	if((uint32_t)*dsrc1 < (uint32_t)*ddest ){
        uint32_t off = id_src2->imm;
        uint32_t p11 = off % 2;
        off = off >> 1;
        uint32_t p1 = off % 1024;
        off = off >> 10;
        uint32_t p12 = off % 2;
        off = off >> 1;
        uint32_t fin = (p12 << 11) | (p11 << 10) | p1;
        fin = fin * 2;
        int32_t dm = (int32_t) fin;
        dm <<= 32 - 13;
        dm >>= 32 - 13;
        fin = dm;                                                           
        rtl_sext(s, &id_src2->imm, &fin, 4);
        rtl_addi(s, &s->dnpc, &s->pc, id_src2->imm);
    }
}

def_EHelper(blt) {
 	if((int32_t) *dsrc1 < (int32_t) *ddest){
        uint32_t off = id_src2->imm;
        uint32_t p11 = off % 2;
        off = off >> 1;
        uint32_t p1 = off % 1024;
        off = off >> 10;
        uint32_t p12 = off % 2;
        off = off >> 1;
        uint32_t fin = (p12 << 11) | (p11 << 10) | p1;
        fin = fin * 2;
        int32_t dm = (int32_t) fin;
        dm <<= 32 - 13;
        dm >>= 32 - 13;
        fin = dm;
        rtl_sext(s, &id_src2->imm, &fin, 4);
        rtl_addi(s, &s->dnpc, &s->pc, id_src2->imm);
    }
}

def_EHelper(bge) {
	if((int32_t) *dsrc1 >= (int32_t) *ddest){
		uint32_t off = id_src2->imm;
        uint32_t p11 = off % 2;
        off = off >> 1;
        uint32_t p1 = off % 1024;
        off = off >> 10;
        uint32_t p12 = off % 2;
        off = off >> 1;
        uint32_t fin = (p12 << 11) | (p11 << 10) | p1;
		fin = fin * 2;
        int32_t dm = (int32_t) fin;
        dm <<= 32 - 13;
        dm >>= 32 - 13;
        fin = dm;
        rtl_sext(s, &id_src2->imm, &fin, 4);
        rtl_addi(s, &s->dnpc, &s->pc, id_src2->imm);
 	}
}

def_EHelper(bgeu) {
	if((uint32_t) *dsrc1 >= (uint32_t) *ddest){
        uint32_t off = id_src2->imm;
        uint32_t p11 = off % 2;
        off = off >> 1;
        uint32_t p1 = off % 1024;
        off = off >> 10;
        uint32_t p12 = off % 2;
        off = off >> 1;
        uint32_t fin = (p12 << 11) | (p11 << 10) | p1;
        fin = fin * 2;
        int32_t dm = (int32_t) fin;
        dm <<= 32 - 13;
        dm >>= 32 - 13;
        fin = dm;
        rtl_sext(s, &id_src2->imm, &fin, 4);
        rtl_addi(s, &s->dnpc, &s->pc, id_src2->imm);
     }
}

def_EHelper(beq) {
	if(*dsrc1 == *ddest){
		uint32_t off = id_src2->imm;
		uint32_t p11 = off % 2;
		off = off >> 1;
		uint32_t p1 = off % 1024;
		off = off >> 10;
		uint32_t p12 = off % 2;
		off = off >> 1;
		uint32_t fin = (p12 << 11) | (p11 << 10) | p1;
		fin = fin * 2;
		int32_t dm = (int32_t) fin;
        dm <<= 32 - 13;
        dm >>= 32 - 13;
        fin = (uint32_t) dm;
		rtl_sext(s, &id_src2->imm, &fin, 4);
		rtl_addi(s, &s->dnpc, &s->pc, id_src2->imm);
 	}
}

#ifdef CONFIG_TRACE
char func[256] = {'\0'};
char result[8388608] = {'\0'};

char * functrace(vaddr_t dnpc, vaddr_t pc, char * str){
	int flag = 0;
/*	  char *p = sym_data;
        int j = 0;
        for (j=0; j<100; j++)
        {
            printf("%02x", *p);
//          q[j] = *p;
//          printf("%c", q[j]);
            p++;
        }
        printf("\n");*/
	for(int i = 0; i < fnum; i++){
		Elf32_Sym * sym = (Elf32_Sym *) (sym_data + i * sizeof(Elf32_Sym));
		if((sym->st_info & 0xf) != 2){
			continue;
		}
		else{
			if(dnpc == gpr(1)){
				if(pc >= sym->st_value && pc < sym->st_value + sym->st_size){
					flag = 1;
					int name = sym->st_name;
					char *p = sign_data + name;
					strcpy(str, p);
					sprintf(result, "0x%x:", pc);
					int len = strlen(result);
					//memset((result + len), ' ', call);
                    //sprintf((result + call + len), "Ret [%s]\n", str);
					sprintf((result + len), "Ret [%s]\n", str);
					ftrace_write("%s", result);
					printf("%s", result);
					call--;
					break;
				}
			}else{
				if(dnpc >= sym->st_value && dnpc < sym->st_value + sym->st_size){
					flag = 1;
					call++;
					int name = sym->st_name;
                    char *p = sign_data + name;
					strcpy(str, p);
					sprintf(result, "0x%x:", pc);
					int len = strlen(result);
					//memset((result + len), ' ', call);
					//sprintf((result + call + len), "Call [%s @0x%x]\n", str, dnpc);
					sprintf((result + len), "Call [%s]\n", str);
//                    printf("%s", result);
					ftrace_write("%s", result);
					printf("%s", result);
					break;
				}
			}
//			printf("%x\n", (sym->st_info & 0xf));
/*			if(dnpc >= sym->st_value && dnpc < sym->st_value + sym->st_size){
				flag = 1;
				if(dnpc == gpr(1)){
//					strcpy(str, "ret from ");
					int name = sym->st_name;
                    char *p = sign_data + name;
                    strcpy(str, p);
                    printf("Return from %s, pc = %x\n", str, pc);
//                    memset(str, '\0', 64);
				}else{
					int name = sym->st_name;
					char *p = sign_data + name;
					strcpy(str, p);
					printf("Call %s, pc = %x\n", str, dnpc);
//					memset(str, '\0', 64);
				}
				break;
			}else{
				if(pc >= sym->st_value && pc < sym->st_value + sym->st_size){
				}
			}*/
		}
	}
	if(pc >= 0x83000000 || dnpc >= 0x83000000)
		flag = 1;
	if(flag == 0){
		strcpy(str, "???");
		printf("Not found func %s, pc = %x\n", str, dnpc);
	}
	return result;
}
#endif

def_EHelper(jal) {
//	printf("!!jal ddest = %x, pc = %x\n", *ddest, s->pc)
	uint32_t t = s->pc + 4;
//	rtl_addi(s, ddest, &s->pc, 4);
	uint32_t imm = id_src1->imm >> 12;
	uint32_t p12 = imm % 256;
	imm = imm >> 8;
	uint32_t p11 = imm % 2;
	imm = imm >> 1;
	uint32_t p1 = imm % 1024;
	imm = imm >> 10;
	uint32_t p20 = imm % 2;
	imm = imm >> 1;
	uint32_t fin = p20 << 8;
	fin = fin + p12;
	fin = fin << 1;
	fin = fin + p11;
	fin = fin << 10;
	fin = fin + p1;
	int32_t dm = (int32_t) fin;
    dm <<= 32 - 20;
    dm >>= 32 - 20;
    fin = dm;
	fin = fin * 2;
	rtl_sext(s, &id_src1->imm, &fin, 4);
	rtl_addi(s, &s->dnpc, &s->pc, id_src1->imm);
	rtl_addi(s, ddest, &t, 0);
#ifdef CONFIG_TRACE
	functrace(s->dnpc, s->pc, func);
#endif
//    printf("jal ret pc = %x\n", s-> dnpc);
}

def_EHelper(jalr) {
//	printf("!!jalr ddest = %x\n", *ddest);
    uint32_t t = s->pc + 4;
	int32_t dm = (int32_t) id_src2->imm;
    dm <<= 32 - 12;
    dm >>= 32 - 12;
    id_src2->imm = dm;
//	printf("ra = %x, offset = %x\n", *dsrc1, id_src2->imm);
	rtl_sext(s, &id_src2->imm, &id_src2->imm, 4);
	rtl_addi(s, &s->dnpc, dsrc1, id_src2->imm);
	s->dnpc = s->dnpc & 0xfffffffe;
//	printf("jalr before addi pc = %x\n", s->dnpc);
	rtl_addi(s, ddest, &t, 0);
//	printf("!!jalr dsrc1 = %x\n", s->pc);
//	if(*ddest == 0 && id_src2->imm == 0 && *dsrc1 == gpr(1)){
#ifdef CONFIG_TRACE
	functrace(s->dnpc, s->pc, func);
#endif
//	printf("jalr ret pc = %x\n", s->dnpc);
//	}
}
