#include <am.h>
#include <nemu.h>
#include <klib.h>
#include <riscv/riscv.h>

static AddrSpace kas = {};
static void *(*pgalloc_usr)(int) = NULL;
static void (*pgfree_usr)(void *) = NULL;
static int vme_enable = 0;
//static int init_flag = 0;

static Area segments[] = { // Kernel memory mappings
    NEMU_PADDR_SPACE};

#define USER_SPACE RANGE(0x40000000, 0x80000000)

static inline void set_satp(void *pdir)
{
  uintptr_t mode = 1ul << (__riscv_xlen - 1);
  asm volatile("csrw satp, %0" : : "r"(mode | ((uintptr_t)pdir >> 12)));
}

static inline uintptr_t get_satp()
{
  uintptr_t satp;
  asm volatile("csrr %0, satp" : "=r"(satp));
  return satp << 12;
}

bool vme_init(void *(*pgalloc_f)(int), void (*pgfree_f)(void *))
{
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;
  int i;

  kas.ptr = pgalloc_f(PGSIZE);

  //  printf("In the vme_init before map!\n");

  for (i = 0; i < LENGTH(segments); i++)
  {
    void *va = segments[i].start;
    for (; va < segments[i].end; va += PGSIZE)
    {
      map(&kas, va, va, 0);
  //    init_flag = 1;
    }
  }

//  printf("satp before = %x, kas.ptr = %x!\n", get_satp(), kas.ptr);

  set_satp(kas.ptr);
//  printf("satp after = %x!\n", get_satp());
  vme_enable = 1;

  return true;
}

void protect(AddrSpace *as)
{
  PTE *updir = (PTE *)(pgalloc_usr(PGSIZE));
  as->ptr = updir;
  as->area = USER_SPACE;
  as->pgsize = PGSIZE;
  // map kernel space
  memcpy(updir, kas.ptr, PGSIZE);
}

void unprotect(AddrSpace *as)
{
}

void __am_get_cur_as(Context *c)
{
  c->pdir = (vme_enable ? (void *)get_satp() : NULL);
//  printf("__am_get_cur_as c->pdir = %x\n", c->pdir);
}

void __am_switch(Context *c)
{
//  printf("__am_switch c->pdir = %x\n", c->pdir);
  if (vme_enable && c->pdir != NULL)
  {
    set_satp(c->pdir);
  }
//  printf("__am_switch satp = %x\n", get_satp());
}

#define PGDIR_MASK 0xffc00000
#define PTE_MASK 0xfffffc00u

#define PTE_V 0x01
#define PTE_A 0x40
#define PTE_D 0x80

void map(AddrSpace *as, void *va, void *pa, int prot)
{
//  printf("In the map paddr = %x, vaddr = %x!\n", pa, va);
  va = (void *)((uintptr_t)va & ~0xfff);
  pa = (void *)((uintptr_t)pa & ~0xfff);
  uintptr_t *page_dir = as->ptr + (((uintptr_t)va & PGDIR_MASK) >> 20);

//  printf("va  = %x, page_dir = %x ", (((uintptr_t)va & PGDIR_MASK) >> 20), page_dir);
//  page_dir = page_dir + (((uintptr_t)va & PGDIR_MASK) >> 22);
//  printf("Page_dir_entry addr: %x!\t", page_dir);

//  if (init_flag == 0)
//    *page_dir = 0;
  if (!(*page_dir & PTE_V))
  {
    void *newpage = pgalloc_usr(4096);
    //    printf("newpage addr: %x", newpage);
    *page_dir = (*page_dir & ~PTE_MASK) | (((uintptr_t)newpage >> 2) & PTE_MASK);
    *page_dir = *page_dir | PTE_V;
  }
  //  printf("Page_dir: %x!", *page_dir);

  uintptr_t *page_table = (uintptr_t *)(((*page_dir & PTE_MASK) << 2) + (((uintptr_t)va & 0x003ff000) >> 10));
//  if (((uintptr_t)page_table & 0x00000fff) == 0xffc)
//    init_flag = 0;
  *page_table = (PTE_MASK & ((uintptr_t)pa >> 2)) | (PTE_V); //| (prot ? PTE_U : 0);
  printf("Page_table addr: %x!\n", page_table);
}

Context *ucontext(AddrSpace *as, Area kstack, void *entry)
{
  Context *c = kstack.end - sizeof(Context);
  c->mepc = (uintptr_t)entry;
  c->pdir = as->ptr;
  return c;
}
