def_EHelper(lw) {
	int32_t dm = (int32_t) id_src2->imm;
    dm <<= 32 - 12;
    dm >>= 32 - 12;
    id_src2->imm = dm;
//	printf("lw src = %x\n", *dsrc1);
    rtl_lms(s, ddest, dsrc1, id_src2->imm, 4);
//  printf("lw = %x\n", *ddest);
}

def_EHelper(lh) {
	int32_t dm = (int32_t) id_src2->imm;
    dm <<= 32 - 12;
    dm >>= 32 - 12;
    id_src2->imm = dm;
//	printf("lh src= %x\n", *dsrc1);
	rtl_lms(s, ddest, dsrc1, id_src2->imm, 4);
	*ddest = *ddest & 0x0000ffff;
	int32_t de = (int32_t) *ddest;
    de <<= 32 - 16;
    de >>= 32 - 16;
    *ddest = de;
//	printf("lh = %x\n", *ddest);
//	rtl_sext(s, ddest, ddest, 2);
}

def_EHelper(lhu) {
	int32_t dm = (int32_t) id_src2->imm;
    dm <<= 32 - 12;
    dm >>= 32 - 12;
    id_src2->imm = dm;
//	printf("lhu src= %x\n", *dsrc1);
	rtl_lm(s, ddest, dsrc1, id_src2->imm, 2);
//	printf("lhu = %x\n", *ddest);
//	*ddest = *ddest & 0x0000ffff;
}

def_EHelper(lbu) {                
    int32_t dm = (int32_t) id_src2->imm;
    dm <<= 32 - 12;
    dm >>= 32 - 12;
    id_src2->imm = dm;
//    printf("lbu src= %x, pc = %x, dest = %x\n", *dsrc1, s->pc, *ddest);
    rtl_lm(s, ddest, dsrc1, id_src2->imm, 1);
//  *ddest = *ddest & 0x000000ff;
}

def_EHelper(lb) {
	int32_t dm = (int32_t) id_src2->imm;
    dm <<= 32 - 12;
    dm >>= 32 - 12;
    id_src2->imm = dm;
//	printf("lbu src= %x\n", *dsrc1);
	rtl_lms(s, ddest, dsrc1, id_src2->imm, 1);
	*ddest = *ddest & 0x000000ff;
    int32_t de = (int32_t) *ddest;
    de <<= 32 - 8;
    de >>= 32 - 8;
    *ddest = de;
	//	printf("lbu = %x\n", *ddest);
//	*ddest = *ddest & 0x000000ff;
}

def_EHelper(sh) {
	int32_t dm = (int32_t) id_src2->imm;
    dm <<= 32 - 12;
    dm >>= 32 - 12;
    id_src2->imm = dm;
//	printf("sh src= %x\n", *dsrc1);
	rtl_sm(s, ddest, dsrc1, id_src2->imm, 2);
//	printf("sh = %x\n", *ddest);
//	*ddest = *ddest & 0x0000ffff;
}

def_EHelper(sb) {
	int32_t dm = (int32_t) id_src2->imm;
    dm <<= 32 - 12;
    dm >>= 32 - 12;
    id_src2->imm = dm;
//	printf("sb src= %x\n", *dsrc1);
	rtl_sm(s, ddest, dsrc1, id_src2->imm, 1);
//	printf("sb = %x\n", *ddest);
//	*ddest = *ddest & 0x000000ff;
}

def_EHelper(sw) {
	int32_t dm = (int32_t) id_src2->imm;
    dm <<= 32 - 12;
    dm >>= 32 - 12;
    id_src2->imm = dm;
//	printf("sw src= %x\n", *dsrc1);
  rtl_sm(s, ddest, dsrc1, id_src2->imm, 4);
//  printf("sw = %x\n", *ddest);
}
