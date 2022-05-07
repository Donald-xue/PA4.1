def_EHelper(lui) {
	rtl_sext(s, &id_src1->imm, &id_src1->imm, 4);
    rtl_li(s, ddest, id_src1->imm);
}

def_EHelper(xori){
	rtl_xori(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(ori){
    rtl_ori(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(srl){
	rtl_srl(s, ddest, dsrc1, dsrc2);
}

def_EHelper(remu){
	if(*dsrc2 != 0)
		rtl_divu_r(s, ddest, dsrc1, dsrc2);
}

def_EHelper(and){
	rtl_and(s, ddest, dsrc1, dsrc2);
}

def_EHelper(sra){
	rtl_sra(s, ddest, dsrc1, dsrc2);
}

def_EHelper(sll){
	rtl_sll(s, ddest, dsrc1, dsrc2);
}

def_EHelper(or){
	rtl_or(s, ddest, dsrc1, dsrc2);
}

def_EHelper(xor){
	rtl_xor(s, ddest, dsrc1, dsrc2);
}

def_EHelper(andi){
	int32_t dm = (int32_t) id_src2->imm;
    dm <<= 32 - 12;
    dm >>= 32 - 12;
    id_src2->imm = dm;
    rtl_sext(s, &id_src2->imm, &id_src2->imm, 4);
	rtl_andi(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(sub){
	rtl_sub(s, ddest, dsrc1, dsrc2);
}

def_EHelper(slt) {
	if(*(int32_t *)dsrc1 < *(int32_t *)dsrc2){
		*ddest = 1;
	}else{
		*ddest = 0;
	}
}

def_EHelper(rem) {
	if(*dsrc2 != 0)
		rtl_divs_r(s, ddest, dsrc1, dsrc2);
}

def_EHelper(add) {
	rtl_add(s, ddest, dsrc1, dsrc2);
}

def_EHelper(auipc) {
	rtl_sext(s, &id_src1->imm, &id_src1->imm, 4);
	rtl_addi(s, ddest, &s->pc, id_src1->imm);
}

def_EHelper(mul) {
	rtl_mulu_lo(s, ddest, dsrc1, dsrc2);
}

def_EHelper(mulh) {
    rtl_muls_hi(s, ddest, dsrc1, dsrc2);	
}

def_EHelper(mulhu) {
    rtl_mulu_hi(s, ddest, dsrc1, dsrc2);    
}

def_EHelper(div) {
	if(*dsrc2 != 0)
		rtl_divs_q(s, ddest, dsrc1, dsrc2);
}

def_EHelper(divu) {
	if(*dsrc2 != 0)
		rtl_divu_q(s, ddest, dsrc1, dsrc2);
}

def_EHelper(li) {
	int32_t dm = (int32_t) id_src1->imm >> 12;
    dm <<= 32 - 12;
    dm >>= 32 - 12;
    id_src1->imm = dm;
	rtl_li(s, ddest, id_src1->imm);
}

def_EHelper(sltu) {
	if((uint32_t) *dsrc1 < (uint32_t) *dsrc2){
		*ddest = 1;
	}else{
		*ddest = 0;
	}
}

def_EHelper(sltiu) {
	int32_t dm = (int32_t) id_src2->imm;
    dm <<= 32 - 12;
    dm >>= 32 - 12;
    id_src2->imm = dm;
	rtl_sext(s, &id_src2->imm, &id_src2->imm, 4);
	if((uint32_t) *dsrc1 < (uint32_t) id_src2->imm){
		*ddest = 1;
	}else{
		*ddest = 0;
	}
}

def_EHelper(slti) {
    int32_t dm = (int32_t) id_src2->imm;
    dm <<= 32 - 12;
    dm >>= 32 - 12;
    id_src2->imm = dm;
    rtl_sext(s, &id_src2->imm, &id_src2->imm, 4);
    if((int32_t) *dsrc1 < (int32_t) id_src2->imm){
        *ddest = 1;
    }else{
         *ddest = 0;
    }
}

def_EHelper(srli) {
	id_src2->imm = id_src2->imm & 0x01f;
    *ddest = *dsrc1 >> id_src2->imm;
}

def_EHelper(slli) {
	id_src2->imm = id_src2->imm & 0x01f;
	*ddest = *dsrc1 << id_src2->imm;
}

def_EHelper(srai) {
	id_src2->imm = id_src2->imm & 0x01f;
	*ddest = *(int32_t *)dsrc1 >> id_src2->imm;
}

def_EHelper(addi) {
	int32_t dm = (int32_t) id_src2->imm;
    dm <<= 32 - 12;
    dm >>= 32 - 12;
    id_src2->imm = dm;
	rtl_sext(s, &id_src2->imm, &id_src2->imm, 4);
	rtl_addi(s, ddest, dsrc1, id_src2->imm);
}
