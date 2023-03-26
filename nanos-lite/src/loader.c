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
void map(AddrSpace *as, void *va, void *pa, int prot);

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
void* new_page(size_t nr_page);

#define PAGESIZE 4096

static void *alloc_page(AddrSpace *as, uintptr_t va, size_t memsz){
  size_t num = ((va + memsz - 1) >> 12) - (va >> 12) + 1;
  void * page_start = new_page(num);

  printf("Loaded Segment from [%x to %x)\n", va, va + memsz);

  for (int i = 0; i < num; i++){
    map(as, (void *)((va & ~0xfff) + i * PAGESIZE), (void *)(page_start + i * PAGESIZE), 1);
  }
  return page_start;
}

static uintptr_t loader(PCB *pcb, const char *filename) {
//  TODO();
  AddrSpace *as = &pcb->as;
  uint32_t fd;
//  printf("loader fs_open %s\n", filename);
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
    uintptr_t vaddr;
    uintptr_t paddr;
	  if( phdr[i].p_type == PT_LOAD ){
//		  printf("第%d个信息!!\n", i);
      vaddr = phdr[i].p_vaddr;
      paddr = (uintptr_t)alloc_page(as, vaddr, phdr[i].p_memsz);
      fs_setoffset(fd, phdr[i].p_offset);
//      printf("Read to addr: %p\n", (void *)(paddr + (vaddr & 0xfff)));
		  fs_read(fd, (void *)(paddr + (vaddr & 0xfff)), phdr[i].p_memsz);
//		  printf("第%d个信息!!\n", i);
//		  ramdisk_read((void *)phdr[i].p_vaddr, phdr[i].p_offset + fs_getdiskoff(fd), phdr[i].p_memsz);
		  memset((void *)(paddr + (vaddr & 0xfff) + phdr[i].p_filesz), 0, phdr[i].p_memsz-phdr[i].p_filesz);
//      printf("Content after read: %x, read count = %d\n", *((int *)(vaddr + (0x3e8))), count);
//		  printf("第%d个0!!\n", i);
      if (phdr[i].p_filesz < phdr[i].p_memsz){// 应该是.bss节
        //做一个向上的4kb取整数
  //      printf("Setting .bss end %x\n", phdr[i].p_vaddr + phdr[i].p_memsz);
        //pcb->max_brk = MAX(pcb->max_brk, ROUNDUP(section_entry.p_vaddr + section_entry.p_memsz, 0xfff));
        pcb->max_brk = ROUNDUP(phdr[i].p_vaddr + phdr[i].p_memsz, 0xfff);
        printf("Setting .bss end %x\n", pcb->max_brk);
      }
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

