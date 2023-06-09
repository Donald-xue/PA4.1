#include <isa.h>
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"

static int is_batch_mode = false;
extern NEMUState nemu_state;
extern word_t paddr_read(paddr_t addr, int len);
extern paddr_t host_to_guest(uint8_t *haddr);
extern void insertwp(char *arg);
extern void displaywp();
extern void deletewp(int No);

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args) {
	nemu_state.state = NEMU_QUIT;
   	return -1;
}

static int cmd_x(char *args);
static int cmd_help(char *args);
static int cmd_si(char *args);
static int cmd_info(char *args);
static int cmd_p(char *args);
static int cmd_w(char *args);
static int cmd_d(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Step execution", cmd_si},
  { "info", "Print status of register", cmd_info},
  { "x", "Examine memory", cmd_x },
  { "p", "Calculation", cmd_p },
  { "w", "Set watchpoint", cmd_w},
  { "d", "Delete watchpoint", cmd_d},

  /* TODO: Add more commands */

};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_d(char *args){
	char *arg = strtok(NULL, "\n");
	int No;
	sscanf(arg, "%d", &No);
	deletewp(No);
	return 0;
}

static int cmd_w(char *args){
	char *arg = strtok(NULL, "\n");
	insertwp(arg);
	return 0;
}

static int cmd_p(char *args){
	char *arg = strtok(NULL, "\n");
	bool success = true;
	expr(arg, &success);
	return 0;
}

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

static int cmd_x(char *args){
	char *arg1 = strtok(NULL, " ");
	char *arg2 = strtok(NULL, " ");
	int num = atoi(arg1);
	paddr_t addr;
	sscanf(arg2, "%x", &addr);
	int read;

	for ( int i=0; i < num; i++ ){
		read = paddr_read(addr+i*4, 4);
		if( i%4 == 0 ) {
			if(i != 0) printf("\n");
			printf("0x%x:\t", addr);
		}
		printf("0x%08x\t", read);
	}
	printf("\n");
	return 0;
}

static int cmd_si(char *args){
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int step;
	if(arg == NULL)
	{ step = 1; }
	else{
	step = atoi(arg);
    if(step < -1){                                                          
         printf("Error，N is an integer greater than or equal to -1\n");  
         return 0;
	}
	}
	cpu_exec((uint64_t)step);
	return 0;
}

static int cmd_info(char *args){
	char *arg = strtok(NULL, " ");
	if(strcmp (arg, "r") == 0){
		isa_reg_display();
		return 0;
	}
	if(strcmp (arg, "w") == 0 ){
		displaywp();
		return 0;
	}
	else{
		return -1;
	}
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
