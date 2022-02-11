#include <common.h>

void init_monitor(int, char *[]);
void am_init_monitor();
void engine_start();
int is_exit_status_bad();
extern word_t expr(char *e, bool *success);

static void test(){
	FILE *fp = fopen("/home/xuezeqian/ics2021/nemu/tools/gen-expr/input", "r");
	assert(fp != NULL);
	char gets[65536] = {};
	char *success = gets;
	int right = 0;
	int wrong = 0;
	while (success){
		success = fgets(gets, 65536, fp);
		if(success == NULL)  break;
		unsigned int result = atoi(strtok(gets, " "));
		char *buf;
	    buf = strtok(NULL, "\n");
		word_t cal = 0;
		cal = expr(buf, NULL);
		if(cal == result){
			right++;
		}
		if(cal != result){
			wrong++;
		}
	}
	printf("right = %d, wrong = %d", right, wrong);
}

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
#ifdef CONFIG_TARGET_AM
  am_init_monitor();
#else
  init_monitor(argc, argv);
#endif

  test();
  /* Start engine. */
  engine_start();

  return is_exit_status_bad();
}
