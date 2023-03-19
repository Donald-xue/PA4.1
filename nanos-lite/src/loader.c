#include <proc.h>
#include <elf.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

extern size_t ramdisk_read(void *buf, size_t offset, size_t len);
extern size_t ramdisk_write(const void *buf, size_t offset, size_t len);
extern size_t get_ramdisk_size();

#if defined(__ISA_AM_NATIVE__)
# define EXPECT_TYPE EM_X86_64
#elif defined(__ISA_X86__)
# define EXPECT_TYPE EM_X86_64  // see /usr/include/elf.h to get the right type
#elif defined(__ISA_RISCV32__)
# define EXPECT_TYPE EM_RISCV
#elif defined(__ISA_MIPS32__)
# define EXPECT_TYPE EM_MIPS
#else
# error Unsupported ISA
#endif

extern int fs_open(const char *pathname);
extern size_t fs_read(int fd, void *buf, size_t len);
extern void fs_setoffset(int fd, size_t offset);
extern size_t fs_getdiskoff(int fd);

static uintptr_t loader(PCB *pcb, const char *filename) {
//  TODO();
  uint32_t fd;
  printf("loader fs_open %s\n", filename);
  fd = fs_open(filename);
  Elf_Ehdr ehdr;
//  uint32_t count;
  fs_read(fd, &ehdr, sizeof(Elf_Ehdr));
//  ramdisk_read(&ehdr, fs_getdiskoff(fd), sizeof(Elf_Ehdr));
  Elf_Ehdr *elf = &ehdr;
  assert(*(uint32_t *)elf->e_ident == 0x464c457f);
  //printf("EXPECT_TYPE = %d, ehdr.e_machine = %d\n", EXPECT_TYPE, ehdr.e_machine);
  assert(EXPECT_TYPE == ehdr.e_machine);
  Elf_Phdr phdr[ehdr.e_phnum];
  fs_read(fd, phdr, sizeof(Elf_Phdr)*ehdr.e_phnum);
//  printf("reach here !!!!!!!!!!!!!!!!!!!!\n");
//  ramdisk_read(phdr, ehdr.e_ehsize + fs_getdiskoff(fd), sizeof(Elf_Phdr)*ehdr.e_phnum);
   for(int i = 0; i < ehdr.e_phnum; i++){
	  if( phdr[i].p_type == PT_LOAD ){
//		  printf("第%d个信息!!\n", i);
          fs_setoffset(fd, phdr[i].p_offset);		  
		  fs_read(fd, (void *)phdr[i].p_vaddr, phdr[i].p_memsz);
//		  printf("第%d个信息!!\n", i);
//		  ramdisk_read((void *)phdr[i].p_vaddr, phdr[i].p_offset + fs_getdiskoff(fd), phdr[i].p_memsz);
		  memset((void *)(phdr[i].p_vaddr+phdr[i].p_filesz), 0, phdr[i].p_memsz-phdr[i].p_filesz);
//		  printf("第%d个0!!\n", i);
 	  }
  }
  return ehdr.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

uintptr_t loader_call (PCB *pcb, const char *filename) {
  uintptr_t ptr = loader(pcb, filename);
  return ptr;
}

