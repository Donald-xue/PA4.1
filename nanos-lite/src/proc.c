#include <proc.h>
#include <am.h>

#define MAX_NR_PROC 4
#define NR_PAGE 8
#define PAGESIZE 4096

extern void naive_uload(PCB *pcb, const char *filename);
uintptr_t loader_call (PCB *pcb, const char *filename);
void* new_page(size_t nr_page);
void switch_boot_pcb();
int fs_open(const char *pathname);
void protect(AddrSpace *as);

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

void context_kload(PCB *pcb, void (*entry)(void *), void *arg){
  Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);
  pcb->cp = kcontext(stack, entry, arg);
//  pcb->cp->GPRx  = &pcb->stack;
}

static size_t align_4_bytes(size_t size){
  if (size & 0x3)
    return (size & (~0x3)) + 0x4;
  return size;
}

void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]){
  AddrSpace *as = &pcb->as;
  protect(as);
//  printf("In the context_uload with filename: %s\n", filename);
  int argc = 0, envc = 0;
  if (envp){
    for (; envp[envc]; ++envc){}
  }
  if (argv){
    for (; argv[argc]; ++argc){}
  }
  
  void * alloced_page = new_page(NR_PAGE) + NR_PAGE * PAGESIZE; //得到栈顶

  map(as, as->area.end - 8 * PAGESIZE, alloced_page - 8 * PAGESIZE, 1); 
  map(as, as->area.end - 7 * PAGESIZE, alloced_page - 7 * PAGESIZE, 1);
  map(as, as->area.end - 6 * PAGESIZE, alloced_page - 6 * PAGESIZE, 1); 
  map(as, as->area.end - 5 * PAGESIZE, alloced_page - 5 * PAGESIZE, 1);
  map(as, as->area.end - 4 * PAGESIZE, alloced_page - 4 * PAGESIZE, 1); 
  map(as, as->area.end - 3 * PAGESIZE, alloced_page - 3 * PAGESIZE, 1);
  map(as, as->area.end - 2 * PAGESIZE, alloced_page - 2 * PAGESIZE, 1); 
  map(as, as->area.end - 1 * PAGESIZE, alloced_page - 1 * PAGESIZE, 1); 
  
  char * ustack = (char *)(alloced_page - 4);

  char *argv_ustack[argc];
  for(int i = 0; i < argc; i++){
    ustack -= align_4_bytes(strlen (argv[i]) + 1);
    argv_ustack[i] = ustack;
    strcpy(ustack, argv[i]);
  }

  char *envp_ustack[envc];
  for(int i = 0; i < envc; i++){
    ustack -= align_4_bytes(strlen (envp[i]) + 1);
    envp_ustack[i] = ustack;
    strcpy(ustack, envp[i]);
  }

  uintptr_t *ptr = (uintptr_t *)ustack;
  ptr -= 1;
  *ptr = 0;

  
  ptr -= envc;
  for (int i = 0; i < envc; ++i){
    ptr[i] = (intptr_t)(envp_ustack[i]);
  }

  ptr -= 1;
  *ptr = 0;
  ptr -= argc;
  for (int i = 0; i < argc; ++i){
    ptr[i] = (intptr_t)(argv_ustack[i]);
  }

//  printf("In the context_uload ptr: %p\n", ptr);
  ptr -= 1;
  *ptr = argc;

//  ptr -= 1;
//  *ptr = 0;

  uintptr_t entry = loader_call(pcb, filename);
  Log("Jump to entry = %p", entry);
  Area karea;
  karea.start = &pcb->cp;
  karea.end = &pcb->cp + STACK_SIZE;
//  stack.start = heap.end;//pcb->stack;
//  stack.end = stack.start + sizeof(pcb->stack);
  pcb->cp = ucontext(as, karea, (void *)entry);

  pcb->cp->gpr[2]  = (uintptr_t)ptr - (uintptr_t)alloced_page + (uintptr_t)as->area.end;

  pcb->cp->GPRx  = (uintptr_t)ptr - (uintptr_t)alloced_page + (uintptr_t)as->area.end + 4;
  //printf("In the context_uload ptr: %p\n", ptr);
}

int execve(const char *filename, char *const argv[], char *const envp[]) {
  if (fs_open(filename) == -1){// 文件不存在
    return -2;
  }
  printf("Loading from %s ...\n", filename);
  context_uload(&pcb[1], filename, argv, envp);
  switch_boot_pcb();
//  printf("In the execve after switch_boot_pcb\n");
  yield();
  return 0;
}

void switch_boot_pcb() {
  current = &pcb_boot; 
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    if(j % 20000 == 0)
    Log("Hello World from Nanos-lite with arg '%s' for the %dth time!", (uintptr_t)arg, j);
    j ++;
    yield();
  }
}

void init_proc() {
  char *argv1[] = {"/bin/menu", NULL};

  context_kload(&pcb[0], hello_fun, "1");
  context_uload(&pcb[1], "/bin/menu", argv1, NULL);
//  printf("pcb[0] is %x\n", &pcb[0]);
  
  switch_boot_pcb();

  Log("Initializing processes...");
//  printf("current pcb is %x\n", current);

  // load program here
  //naive_uload(NULL, "/bin/nterm");
}

Context* schedule(Context *prev) {
//  printf("Before schedule current pcb is %x\n", current);
  current->cp = prev;
  current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);
//  current = &pcb[0];
//  printf("current pcb is pcb[0] %x\n", current);
  return current->cp;
}
