#include <isa.h>
#include <memory/paddr.h>
#include <memory/vaddr.h>
#include <isa-def.h>

#define PGDIR_MASK 0xffc00000u
#define PGTAB_MASK 0x003ff000u
#define PTE_MASK 0xfffffc00u
#define OFFSET_MASK 0x00000fffu

#define PTE_V 0x01
#define PTE_A 0x40
#define PTE_D 0x80

paddr_t isa_mmu_translate(vaddr_t vaddr, int len, int type) {
//  printf("vaddr = %x\t", vaddr);
  paddr_t page_dir_entry = (cpu.satp << 12) + (((uintptr_t)vaddr & PGDIR_MASK) >> 20);
//  printf("In isa_mmu_translate, page_dir_entry = %x\n", page_dir_entry);
  uintptr_t page_dir = paddr_read(page_dir_entry, 4);
  Assert(page_dir & PTE_V, "page_dir not valid, vaddr: %x", vaddr);

  paddr_t page_table_entry = ((page_dir & PTE_MASK) << 2) + ((vaddr & PGTAB_MASK) >> 10);
//  printf("In isa_mmu_translate, page_table_entry = %x\n", page_table_entry);
  uintptr_t page_table = paddr_read(page_table_entry, 4);
  Assert(page_table & PTE_V, "page_table not valid, vaddr: %x", vaddr);


  if (type == 0){//读
    paddr_write(page_table_entry, 4, page_table | PTE_A);
  }else if (type == 1){//写
    paddr_write(page_table_entry, 4, page_table | PTE_D);
  }
//  printf("page_table_entry = %x\n", page_table_entry);
  paddr_t paddr = ((page_table & PTE_MASK) << 2) + (vaddr & OFFSET_MASK);
//  printf("paddr = %x, vaddr = %x\n", paddr, vaddr);
//  Assert(paddr == vaddr, "Paddr:%#x not equal to vaddr: %x", paddr, vaddr);  

  return paddr;
}
