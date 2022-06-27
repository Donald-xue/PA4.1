#include <cpu/cpu.h>
#include <cpu/exec.h>
#include <cpu/difftest.h>
#include <isa-all-instr.h>
#include <locale.h>
#include <elf.h>

/* The assembly code of instructions executed is only output to the screen
 * when the number of instructions executed is less than this value.
 * This is useful when you use the `si' command.
 * You can modify this value as you want.
 */
#define MAX_INSTR_TO_PRINT 10

CPU_state cpu = {};
uint64_t g_nr_guest_instr = 0;
static uint64_t g_timer = 0; // unit: us
static bool g_print_step = false;
const rtlreg_t rzero = 0;
rtlreg_t tmp_reg[4];
extern int checkwp();
#ifdef CONFIG_TRACE
static char r0[128] = {'\0'};
static char r1[128] = {'\0'};
static char r2[128] = {'\0'};
static char r3[128] = {'\0'};
static char r4[128] = {'\0'};
static char r5[128] = {'\0'};
static char r6[128] = {'\0'};
static char r7[128] = {'\0'};
static char r8[128] = {'\0'};
static char  r9[128] = {'\0'};
static char r10[128] = {'\0'};
static char r11[128] = {'\0'};
static char r12[128] = {'\0'};
static char r13[128] = {'\0'};
static char r14[128] = {'\0'};
static char r15[128] = {'\0'}; 
static uint32_t ringLen = 0;//已使用的数据长度
static uint32_t ringnum = 0;
static char * rHead = NULL;//环形存储区的首地址
static char * rTail = NULL;//环形存储区的结尾地址
static char * rValid = NULL;//已使用的缓冲区的首地址
static char * rValidTail = NULL;//已使用的缓冲区的尾地址
#define RINGBUF_SIZE  16   //环形缓冲区的长度,可以修改
char *ringbuf[] = {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15};
static int finalring = 0; 
extern Elf32_Shdr *shdr;
extern char *sign_data;
extern char *sym_data;
#endif
int wrong = 0;
//extern int destory_screen;
extern void vga_destory_screen();
extern void free_map();

void device_update();
void fetch_decode(Decode *s, vaddr_t pc);

static void trace_and_difftest(Decode *_this, vaddr_t dnpc) {
#ifdef CONFIG_ITRACE_COND
  if (ITRACE_COND){
	  log_write("%s\n", _this->logbuf);
	  //printf("nemu_state.state = %d!\n", nemu_state.state);
	  if(nemu_state.state != NEMU_RUNNING && nemu_state.state != NEMU_END){
		  //printf("trace & diff\n");
		  finalring = ringnum % RINGBUF_SIZE;
		  char tail[4] = {"<--"};
		  strcat(rValidTail, tail);
		  for(int i = 0; i<RINGBUF_SIZE; i++){
			  ring_write("%s\n", rValid);
			  if(rValid == ringbuf[i])
			  {
				  //if(rValid != ringbuf[15]){
					  //printf("yes\n");
					  rValid = ringbuf[(i + 1) % RINGBUF_SIZE];
				//  }
				  //else
					//  rValid = rHead;
			  }else{
				  //if(1){rValid != ringbuf[15]){
				  finalring++;
				  finalring = finalring % RINGBUF_SIZE;
				  rValid = ringbuf[finalring];
				  //printf("%s\n", rValid);
				  //}
				  //else
			      //		  rValid = rHead;
			  }
		  }
			  free(shdr);
			  free(sign_data);
			  free(sym_data);
	  }
		  if(nemu_state.state == NEMU_END)
			  ring_write("Nothing wrong while running program, no need for iringbuf\n");
  } 
#endif
  if (g_print_step) { IFDEF(CONFIG_ITRACE, puts(_this->logbuf)); }
  IFDEF(CONFIG_DIFFTEST, difftest_step(_this->pc, dnpc));
#ifdef CONFIG_WATCHPOINT
  if(CONFIG_WATCHPOINT){
	  int i;
	  i = checkwp();
	  if(i == 1){
		  nemu_state.state = NEMU_STOP;
		  printf("hit watchpoint!\n");
	  }
  }
#endif
}

#ifdef CONFIG_TRACE_COND
static void iringbuf(vaddr_t dnpc) {
	//printf("iringbuf!\n");
	//if(nemu_state.state != NEMU_RUNNING || nemu_state.state != NEMU_STOP){
          finalring = ringnum % RINGBUF_SIZE;
          char tail[4] = {"<--"};
          strcat(rValidTail, tail);
          for(int i = 0; i<RINGBUF_SIZE; i++){
              ring_write("%s\n", rValid);
              if(rValid == ringbuf[i])
              {
                  //if(rValid != ringbuf[15]){
                      //printf("yes\n");
                      rValid = ringbuf[(i + 1) % 16];
                //  }
                  //else
                    //  rValid = rHead;
              }else{
                  //if(1){rValid != ringbuf[15]){
                  finalring++;
                  finalring = finalring % RINGBUF_SIZE;
                  rValid = ringbuf[finalring];
 //printf("%s\n", rValid);
                  //}
                  //else
                  //          rValid = rHead;
              }
          }
//      }
}
#endif

#include <isa-exec.h>

#define FILL_EXEC_TABLE(name) [concat(EXEC_ID_, name)] = concat(exec_, name),
static const void* g_exec_table[TOTAL_INSTR] = {
  MAP(INSTR_LIST, FILL_EXEC_TABLE)
};

static void fetch_decode_exec_updatepc(Decode *s) {
  fetch_decode(s, cpu.pc);
  s->EHelper(s);
  cpu.pc = s->dnpc;
}

static void statistic() {
  IFNDEF(CONFIG_TARGET_AM, setlocale(LC_NUMERIC, ""));
//  destory_screen = 1;
  vga_destory_screen();
#define NUMBERIC_FMT MUXDEF(CONFIG_TARGET_AM, "%ld", "%'ld")
  Log("host time spent = " NUMBERIC_FMT " us", g_timer);
  Log("total guest instructions = " NUMBERIC_FMT, g_nr_guest_instr);
  if (g_timer > 0) Log("simulation frequency = " NUMBERIC_FMT " instr/s", g_nr_guest_instr * 1000000 / g_timer);
  else Log("Finish running in less than 1 us and can not calculate the simulation frequency");
  //  free(shdr);
//  free(sign_data);
//  free(sym_data);
}

void assert_fail_msg() {
#ifdef CONFIG_TRACE_COND
	iringbuf(cpu.pc);
#endif
  isa_reg_display();
  statistic();
#ifdef CONFIG_TRACE_COND
  free(shdr);
  free(sign_data);
  free(sym_data);
#endif
}

void fetch_decode(Decode *s, vaddr_t pc) {
  s->pc = pc;
  s->snpc = pc;
  int idx = isa_fetch_decode(s);
  s->dnpc = s->snpc;
  s->EHelper = g_exec_table[idx];
#ifdef CONFIG_ITRACE
  char *p = s->logbuf;
  p += snprintf(p, sizeof(s->logbuf), FMT_WORD ":\t0x", s->pc);
  int ilen = s->snpc - s->pc;
  int i;
  uint8_t *instr = (uint8_t *)&s->isa.instr.val;
  for (i = ilen-1; i >= 0; i --) {
    p += snprintf(p, 4, "%02x", instr[i]);
  }
  int ilen_max = MUXDEF(CONFIG_ISA_x86, 8, 4);
  int space_len = ilen_max - ilen;
  if (space_len < 0) space_len = 0;
  space_len = space_len * 3 + 1;
  memset(p, ' ', space_len);
  p += space_len;

  void disassemble(char *str, int size, uint64_t pc, uint8_t *code, int nbyte);
  disassemble(p, s->logbuf + sizeof(s->logbuf) - p,
      MUXDEF(CONFIG_ISA_x86, s->snpc, s->pc), (uint8_t *)&s->isa.instr.val, ilen);
  rHead = ringbuf[0];//
  rTail = ringbuf[15];
  strcpy(s->ringbuf, s->logbuf);
  memset(ringbuf[ringnum % RINGBUF_SIZE], '\0', 128);
  strcpy(ringbuf[ringnum % RINGBUF_SIZE], s->ringbuf);
  ringLen = ringnum % RINGBUF_SIZE;
  ringnum++;
  if(ringnum == 1){
	  rValid = rValidTail = ringbuf[0];
  }else if((ringnum - 1) / RINGBUF_SIZE == 0){
	  rValidTail = ringbuf[ringnum - 1];
  }else{
	  rValid = ringbuf[ringnum % RINGBUF_SIZE];
	  rValidTail = ringbuf[ringLen];
  }
#endif
}

/* Simulate how the CPU works. */
void cpu_exec(uint64_t n) {
    cpu.mstatus = 0x1800;
	g_print_step = (n < MAX_INSTR_TO_PRINT);
  switch (nemu_state.state) {
    case NEMU_END: case NEMU_ABORT:
      printf("Program execution has ended. To restart the program, exit NEMU and run again.\n");
      return;
    default: nemu_state.state = NEMU_RUNNING;
  }

  uint64_t timer_start = get_time();

  Decode s;
  for (;n > 0; n --) {
    fetch_decode_exec_updatepc(&s);
    g_nr_guest_instr ++;
    trace_and_difftest(&s, cpu.pc);
    if (nemu_state.state != NEMU_RUNNING) break;
    IFDEF(CONFIG_DEVICE, device_update());
  }

  uint64_t timer_end = get_time();
  g_timer += timer_end - timer_start;

  switch (nemu_state.state) {
	case NEMU_STOP: break;
    case NEMU_RUNNING: nemu_state.state = NEMU_STOP; break;

    case NEMU_END: case NEMU_ABORT:
      Log("nemu: %s at pc = " FMT_WORD,
          (nemu_state.state == NEMU_ABORT ? ASNI_FMT("ABORT", ASNI_FG_RED) :
           (nemu_state.halt_ret == 0 ? ASNI_FMT("HIT GOOD TRAP", ASNI_FG_GREEN) :
            ASNI_FMT("HIT BAD TRAP", ASNI_FG_RED))),
          nemu_state.halt_pc);
      // fall through
    case NEMU_QUIT: statistic(); 
  } 
}
