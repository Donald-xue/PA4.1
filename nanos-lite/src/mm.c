#include <memory.h>
#include <string.h>
#include <stdint.h>
#include <proc.h>

static void *pf = NULL;

void* new_page(size_t nr_page) {
  void * ret = pf;
  pf = (char*)pf + nr_page * 4096;
//  memset(ret, 0, 4096);
  return ret;
}

#ifdef HAS_VME
static void* pg_alloc(int n) {
  assert(n % (4096) == 0);
  void *ptr = new_page(n / 4096);
  memset(ptr, 0, n);
  return ptr;
}
#endif

void free_page(void *p) {
  panic("not implement yet");
}

extern PCB *current;
/* The brk() system call handler. */
int mm_brk(uintptr_t brk) {
  uintptr_t end = current->max_brk;
  int new_page_num = 0;

  if(brk > end){
    uintptr_t size = brk - end;
    new_page_num = size / 4096 + 1;
    void * alloced_page = new_page(new_page_num);
    for(int i = 0; i < new_page_num; i++){
      map(&current->as, (void *)(end + i * 4096), (void *)(alloced_page + i * 4096), 1);
      printf("brk map va = %x, pa = %x\n", (end + i * 4096), (alloced_page + i * 4096));
    }
    current->max_brk = new_page_num * 4096 + current->max_brk;
    assert(current->max_brk >= brk);
  }
  return 0;
}

void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
