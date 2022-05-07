#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <signal.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"#include <signal.h>\n"
"static int fpe = 0 ;"
"static void sig_fpe(int signo){ "
"  signal(SIGFPE, sig_fpe); "
"  fpe = -1; "
"  printf(\"%%d\", fpe); "
"}"
"int main() { "
"  signal(SIGFPE, sig_fpe); "
"  unsigned result = %s; "
"  if(fpe == 0) "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

static int flag = 0;

//static void sig_fpe(int signo){
//	divzero = 1;
//}

uint32_t choose(uint32_t n){
	//srand((unsigned) time(NULL));
	uint32_t num = rand() % n;
	return num;
}

uint32_t gen_num(){
	uint32_t i;
	char a[2];
	i = choose(10);
	//printf("num = %d\n",i);
	sprintf(a, "%u", i);
	strcat(buf, a);
	return i;
}

void gen(char str){
	uint32_t lSpace = choose(2);
	uint32_t rSpace = choose(2);

	char s[lSpace + 1 + rSpace];
	uint32_t i;

	for (i = 0; i < lSpace; i++) s[i] = ' ';
	s[i++] = str;
	for (;i < lSpace + 1 + rSpace; i++) s[i] = ' ';
    s[lSpace + 1 + rSpace] = '\0';
	//if(str == '(')
	strcat(buf, s);
	//if(str == ')')
	//	strcat
}

void gen_rand_op(){
	int i = choose(4);
	//printf("rand_op = %d\n", i);
	switch(i){
		case 0: gen('+'); break;
		case 1: gen('-'); break;
		case 2: gen('*'); break;
		case 3: gen('/'); break;
	}
}

static void gen_rand_expr() {
  if(flag == 0){
	memset(buf, '\0', 65536);
	flag++;
  }
  int i = choose(3);
  if(buf[65500] != '\0')
	  i = 0;
  switch(i){
	  case 0: gen_num(); break;
	  case 1: gen('('); gen_rand_expr(); gen(')'); break;
	  default: gen_rand_expr(); gen_rand_op(); gen_rand_expr(); break;
  }
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  //signal(SIGFPE, sig_fpe);
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  //printf("loop = %d\n", loop);
  int i;
  for (i = 0; i < loop; i ++) {
    gen_rand_expr();

//	printf("buf = %s\n", buf);
	
    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0){
		i--;
		memset(buf, '\0', 65536);
		continue;
	}

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    fscanf(fp, "%d", &result);
	if(result == -1){
		pclose(fp);
		memset(buf, '\0', 65536);
		i--;
		continue;
	}
	//printf("[loop %d]\t ", i);
    pclose(fp);

	//if(divzero == 1){
	//	divzero = 0;
	//	memset(buf, '\0', 65536);
	//	continue;
	//}

    printf("%u %s\n", result, buf);
	memset(buf, '\0', 65536);
  }
  return 0;
}
