#include <common.h>
#include <elf.h>
#include <stdio.h>
#include <unistd.h>

Elf32_Shdr *shdr = NULL;
Elf64_Shdr *shdr64 = NULL;
char *sign_data = NULL;
char *sym_data = NULL;
char *strtab = NULL;
extern uint64_t g_nr_guest_instr;
FILE *log_fp = NULL;
FILE *ring_fp = NULL;
FILE *mem_fp = NULL;
FILE *dev_fp = NULL;
FILE *ftrace_fp = NULL;
FILE *dest_fp = NULL;
int fnum = 0;
int elf64 = 0;

void init_log(const char *log_file) {
  log_fp = stdout;
  if (log_file != NULL) {
    FILE *fp = fopen(log_file, "w");
    Assert(fp, "Can not open '%s'", log_file);
    log_fp = fp;
  }
  Log("Log is written to %s", log_file ? log_file : "stdout");
}

void init_ftrace(const char *ftrace_file, const char *dest_file) {
	ftrace_fp = stdout;
	if (ftrace_file != NULL) {
		FILE *fp = fopen(ftrace_file, "r");
		Assert(fp, "Can not open '%s'", ftrace_file);
		ftrace_fp = fp;
	}else{
		elf64 = 1;
		char *buffer, ffile[128], *ftfile;
		ftfile = ffile;
	if((buffer = getcwd(NULL,0))==NULL){
		assert(0);
	}
	else{
		sprintf(ftfile, "%s", buffer);
		printf("%s\n", ftfile);
		free(buffer);
	}
	strcat(ftfile, "/build/riscv32-nemu-interpreter");
	ftrace_file = ftfile;
	printf("%s\n", ftrace_file);
	//		ftrace_file = "/home/xuezeqian/ics2021/nemu/build/riscv32-nemu-interpreter";
		FILE *fp = fopen(ftrace_file, "r");
        Assert(fp, "Can not open '%s'", ftrace_file);
        ftrace_fp = fp;
	}
	Log("Ftrace is read from %s", ftrace_file ? ftrace_file : "stdout");
	dest_fp = stdout;
	if (dest_file != NULL) {
		FILE *fp = fopen(dest_file, "w");
		Assert(fp, "Can not open '%s'", dest_file);
		dest_fp = fp;
	}
	Log("Ftrace is written to  %s", dest_file ? dest_file : "stdout");
	if(elf64 == 0){
	Elf32_Ehdr elf_head;
	int sta;
	sta = fread(&elf_head, sizeof(Elf32_Ehdr), 1, ftrace_fp);
	Assert(sta == 1, "Error occured while reading the file header!\n");
	if (elf_head.e_ident[0] != 0x7F ||
		elf_head.e_ident[1] != 'E' ||
		elf_head.e_ident[2] != 'L' ||
		elf_head.e_ident[3] != 'F')
	{
//		printf("Not an ELF file!\n");
		Assert(0, "Not an ELF file!\n");
	}
	shdr = (Elf32_Shdr *) malloc (sizeof(Elf32_Shdr) * elf_head.e_shnum + 1);
	if (NULL == shdr)
	{
//		printf("Shdr malloc failed!\n");
		Assert(0, "Shdr malloc failed!\n");
	}
	sta = fseek(ftrace_fp, elf_head.e_shoff, SEEK_SET);
	Assert(sta == 0, "Can not locate the section header!\n");
	sta = fread(shdr, sizeof(Elf32_Shdr) * elf_head.e_shnum, 1, ftrace_fp);
	Assert(sta == 1, "Error occured while reading the section header!\n");
	rewind(ftrace_fp);
	// 将fp指针移到 字符串表偏移位置处
	fseek(ftrace_fp, shdr[elf_head.e_shstrndx].sh_offset, SEEK_SET);
	// 第e_shstrndx项是字符串表 定义 字节 长度 char类型 数组
	char shstrtab[shdr[elf_head.e_shstrndx].sh_size];
	char *temp = shstrtab;
	// 读取内容
	sta = fread(shstrtab, shdr[elf_head.e_shstrndx].sh_size, 1, ftrace_fp);
	Assert(sta == 1, "Error occured while reading the string table!\n");
	for (int i = 0; i < elf_head.e_shnum; i++)
	{
		temp = shstrtab;
		temp = temp + shdr[i].sh_name;
        if (strcmp(temp, ".strtab") != 0) continue;//该section名称
//		printf("节的名称: %s\n", temp);
//		printf("节首的偏移: %x\n", shdr[i].sh_offset);
//		printf("节的大小: %x\n", shdr[i].sh_size);
        sign_data = (char *) malloc (sizeof(char) *shdr[i].sh_size + 1);
		// 依据此段在文件中的偏移读取出
		sta = fseek(ftrace_fp, shdr[i].sh_offset, SEEK_SET);
		Assert(sta == 0, "Can not locate the section header in loop!\n");
		sta = fread(sign_data, sizeof(char)*shdr[i].sh_size, 1, ftrace_fp);
		Assert(sta == 1, "Error occured while reading the string table in loop!\n");
		// 显示读取的内容
/*		char *p = sign_data;
		int j = 0;
		char str[128];
//		strtab = (char *) malloc (sizeof(char) *shdr[i].sh_size + 1);
//		char *q =strtab;
//		printf("%x\n", *strtab);*/
/*		for (j=0; j<shdr[i].sh_size; j++)
		{
		    printf("%02x", *p);
//			q[j] = *p;
//			printf("%c", q[j]);
            p++;
		}
		printf("\n");*/
/*		p = sign_data;
//		printf("result: %s\n", q);
//		for (j=0; j<shdr[i].sh_size; j++){
//			printf("%c", q[j]);
//		}
//		printf("\n");
//		printf("%s", sign_data);*/
	 }
	rewind(ftrace_fp);
	int symtab = 0;
	for (symtab = 0; symtab < elf_head.e_shnum; symtab++)
    {
        temp = shstrtab;
        temp = temp + shdr[symtab].sh_name;
        if (strcmp(temp, ".symtab") != 0) continue;//该section名称
/*        printf("节的名称: %s\n", temp);
        printf("节首的偏移: %x\n", shdr[symtab].sh_offset);
        printf("节的大小: %x\n", shdr[symtab].sh_size);
		printf("表项的大小: %x\n", shdr[symtab].sh_entsize);*/
        sym_data = (char *) malloc (sizeof(char) *shdr[symtab].sh_size + 1);
        // 依据此段在文件中的偏移读取出
        sta = fseek(ftrace_fp, shdr[symtab].sh_offset, SEEK_SET);
        Assert(sta == 0, "Can not locate the section header in loop!\n");
        sta = fread(sym_data, sizeof(char) * shdr[symtab].sh_size, 1, ftrace_fp);
        Assert(sta == 1, "Error occured while reading the string table in loop!\n");
		fnum = sizeof(char) *shdr[symtab].sh_size / sizeof(Elf32_Sym);
//		printf("%x", fnum);
        // 显示读取的内容
/*        char *p = sym_data;
        int j = 0;
        for (j=0; j<shdr[symtab].sh_size; j++)
        {
            printf("%02x", *p);
//          q[j] = *p;
//          printf("%c", q[j]);
            p++;
        }
        printf("\n");
		Elf32_Sym * sym = (Elf32_Sym *) (sym_data + 9 * sizeof(Elf32_Sym));
	    printf("%x\n", sym->st_value);	*/
     }
	}else{
		Elf64_Ehdr elf_head;
	int sta;
	sta = fread(&elf_head, sizeof(Elf64_Ehdr), 1, ftrace_fp);
	Assert(sta == 1, "Error occured while reading the file header!\n");
	if (elf_head.e_ident[0] != 0x7F ||
		elf_head.e_ident[1] != 'E' ||
		elf_head.e_ident[2] != 'L' ||
		elf_head.e_ident[3] != 'F')
	{
//		printf("Not an ELF file!\n");
		Assert(0, "Not an ELF file!\n");
	}
	shdr64 = (Elf64_Shdr *) malloc (sizeof(Elf64_Shdr) * elf_head.e_shnum + 1);
	if (NULL == shdr64)
	{
//		printf("Shdr malloc failed!\n");
		Assert(0, "Shdr64 malloc failed!\n");
	}
	sta = fseek(ftrace_fp, elf_head.e_shoff, SEEK_SET);
	Assert(sta == 0, "Can not locate the section header!\n");
	sta = fread(shdr64, sizeof(Elf64_Shdr) * elf_head.e_shnum, 1, ftrace_fp);
	Assert(sta == 1, "Error occured while reading the section header!\n");
	rewind(ftrace_fp);
	// 将fp指针移到 字符串表偏移位置处
	fseek(ftrace_fp, shdr64[elf_head.e_shstrndx].sh_offset, SEEK_SET);
	// 第e_shstrndx项是字符串表 定义 字节 长度 char类型 数组
	char shstrtab[shdr64[elf_head.e_shstrndx].sh_size];
	char *temp = shstrtab;
	// 读取内容
	sta = fread(shstrtab, shdr64[elf_head.e_shstrndx].sh_size, 1, ftrace_fp);
	Assert(sta == 1, "Error occured while reading the string table!\n");
	for (int i = 0; i < elf_head.e_shnum; i++)
	{
		temp = shstrtab;
		temp = temp + shdr64[i].sh_name;
        if (strcmp(temp, ".strtab") != 0) continue;//该section名称
//		printf("节的名称: %s\n", temp);
//		printf("节首的偏移: %x\n", shdr[i].sh_offset);
//		printf("节的大小: %x\n", shdr[i].sh_size);
        sign_data = (char *) malloc (sizeof(char) *shdr64[i].sh_size + 1);
		// 依据此段在文件中的偏移读取出
		sta = fseek(ftrace_fp, shdr64[i].sh_offset, SEEK_SET);
		Assert(sta == 0, "Can not locate the section header in loop!\n");
		sta = fread(sign_data, sizeof(char)*shdr64[i].sh_size, 1, ftrace_fp);
		Assert(sta == 1, "Error occured while reading the string table in loop!\n");
		// 显示读取的内容
/*		char *p = sign_data;
		int j = 0;
		char str[128];
//		strtab = (char *) malloc (sizeof(char) *shdr[i].sh_size + 1);
//		char *q =strtab;
//		printf("%x\n", *strtab);*/
/*		for (j=0; j<shdr[i].sh_size; j++)
		{
		    printf("%02x", *p);
//			q[j] = *p;
//			printf("%c", q[j]);
            p++;
		}
		printf("\n");*/
/*		p = sign_data;
//		printf("result: %s\n", q);
//		for (j=0; j<shdr[i].sh_size; j++){
//			printf("%c", q[j]);
//		}
//		printf("\n");
//		printf("%s", sign_data);*/
	 }
	rewind(ftrace_fp);
	int symtab = 0;
	for (symtab = 0; symtab < elf_head.e_shnum; symtab++)
    {
        temp = shstrtab;
        temp = temp + shdr64[symtab].sh_name;
        if (strcmp(temp, ".symtab") != 0) continue;//该section名称
/*        printf("节的名称: %s\n", temp);
        printf("节首的偏移: %x\n", shdr[symtab].sh_offset);
        printf("节的大小: %x\n", shdr[symtab].sh_size);
		printf("表项的大小: %x\n", shdr[symtab].sh_entsize);*/
        sym_data = (char *) malloc (sizeof(char) *shdr64[symtab].sh_size + 1);
        // 依据此段在文件中的偏移读取出
        sta = fseek(ftrace_fp, shdr64[symtab].sh_offset, SEEK_SET);
        Assert(sta == 0, "Can not locate the section header in loop!\n");
        sta = fread(sym_data, sizeof(char) * shdr64[symtab].sh_size, 1, ftrace_fp);
        Assert(sta == 1, "Error occured while reading the string table in loop!\n");
		fnum = sizeof(char) *shdr64[symtab].sh_size / sizeof(Elf64_Sym);
//		printf("%x", fnum);
        // 显示读取的内容
/*        char *p = sym_data;
        int j = 0;
        for (j=0; j<shdr[symtab].sh_size; j++)
        {
            printf("%02x", *p);
//          q[j] = *p;
//          printf("%c", q[j]);
            p++;
        }
        printf("\n");
		Elf32_Sym * sym = (Elf32_Sym *) (sym_data + 9 * sizeof(Elf32_Sym));
	    printf("%x\n", sym->st_value);	*/
     }
	}
}

void init_ring(const char *ring_file) {
	ring_fp = stdout;
	if (ring_file != NULL) {
		FILE *fp = fopen(ring_file, "w");
		Assert(fp, "Can not open '%s'", ring_file);
		ring_fp = fp;
	}
	Log("Ringbuf is written to %s", ring_file ? ring_file : "stdout");
}

void init_memtrace(const char *mem_file) {
    mem_fp = stdout;                               
    if (mem_file != NULL) {
        FILE *fp = fopen(mem_file, "w");
        Assert(fp, "Can not open '%s'", mem_file);
        mem_fp = fp;
    }
    Log("mtrace is written to %s", mem_file ? mem_file : "stdout");
}

void init_devtrace(const char *dev_file) {
    dev_fp = stdout;                               
    if (dev_file != NULL) {
        FILE *fp = fopen(dev_file, "w");
        Assert(fp, "Can not open '%s'", dev_file);
        dev_fp = fp;
    }
    Log("dtrace is written to %s", dev_file ? dev_file : "stdout");
}

bool log_enable() {
  return MUXDEF(CONFIG_TRACE, (g_nr_guest_instr >= CONFIG_TRACE_START) &&
         (g_nr_guest_instr <= CONFIG_TRACE_END), false);
}
