#include <isa.h>
#include <memory/paddr.h>

void init_rand();
void init_log(const char *log_file);
void init_ring(const char *ring_file);
void init_devtrace(const char *dev_file);
void init_memtrace(const char *mem_file);
void init_ftrace(const char *ftrace_file, const char *dest_file);
void init_mem();
void init_difftest(char *ref_so_file, long img_size, int port);
void init_device();
void init_sdb();
void init_disasm(const char *triple);

static void welcome() {
  Log("Trace: %s", MUXDEF(CONFIG_TRACE, ASNI_FMT("ON", ASNI_FG_GREEN), ASNI_FMT("OFF", ASNI_FG_RED)));
  IFDEF(CONFIG_TRACE, Log("If trace is enabled, a log file will be generated "
        "to record the trace. This may lead to a large log file. "
        "If it is not necessary, you can disable it in menuconfig"));
  Log("Build time: %s, %s", __TIME__, __DATE__);
  printf("Welcome to %s-NEMU!\n", ASNI_FMT(str(__GUEST_ISA__), ASNI_FG_YELLOW ASNI_BG_RED));
  printf("For help, type \"help\"\n");
}

#ifndef CONFIG_TARGET_AM
#include <getopt.h>

void sdb_set_batch_mode();

static char *log_file = NULL;
static char *ftrace_file = NULL;
static char *diff_so_file = NULL;
static char *img_file = NULL;
static int difftest_port = 1234;

static long load_img() {
  if (img_file == NULL) {
    Log("No image is given. Use the default build-in image.");
    return 4096; // built-in image size
  }

  FILE *fp = fopen(img_file, "rb");
  Assert(fp, "Can not open '%s'", img_file);

  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);

  Log("The image is %s, size = %ld", img_file, size);
  ftrace_write("The image is %s, size = %ld\n", img_file, size);
  ring_write("The image is %s, size = %ld\n", img_file, size);
  mtrace_write("The image is %s, size = %ld\n", img_file, size);
  dtrace_write("The image is %s, size = %ld\n", img_file, size);

  fseek(fp, 0, SEEK_SET);
  int ret = fread(guest_to_host(RESET_VECTOR), size, 1, fp);
  assert(ret == 1);

  fclose(fp);
  return size;
}

static int parse_args(int argc, char *argv[]) {
  const struct option table[] = {
    {"batch"    , no_argument      , NULL, 'b'},
    {"log"      , required_argument, NULL, 'l'},
	{"ftrace"   , required_argument, NULL, 'f'},
    {"diff"     , required_argument, NULL, 'd'},
    {"port"     , required_argument, NULL, 'p'},
    {"help"     , no_argument      , NULL, 'h'},
    {0          , 0                , NULL,  0 },
  };
  int o;
  while ( (o = getopt_long(argc, argv, "-bhl:f:d:p:", table, NULL)) != -1) {
    switch (o) {
      case 'b': sdb_set_batch_mode(); break;
      case 'p': sscanf(optarg, "%d", &difftest_port); break;
      case 'l': log_file = optarg; break; 
	  case 'f': ftrace_file = optarg; break;
      case 'd': diff_so_file = optarg; break;
      case 1: img_file = optarg; return optind - 1;
      default:
        printf("Usage: %s [OPTION...] IMAGE [args]\n\n", argv[0]);
        printf("\t-b,--batch              run with batch mode\n");
        printf("\t-l,--log=FILE           output log to FILE\n");
        printf("\t-d,--diff=REF_SO        run DiffTest with reference REF_SO\n");
        printf("\t-p,--port=PORT          run DiffTest with port PORT\n");
        printf("\n");
        exit(0);
    }
  }
  return 0;
}

void init_monitor(int argc, char *argv[]) {
  /* Perform some global initialization. */

  /* Parse arguments. */
  parse_args(argc, argv);

  /* Set random seed. */
  init_rand();

  /* Open the log file. */
  init_log(log_file);
#ifdef CONFIG_TRACE
  char * log = log_file;
#endif
//  char r[128] = {'\0'};
//  strcpy(r, log);
//  printf("%s\n", r);

#ifdef CONFIG_TRACE
  /* Open the ftrace file. */
  char dest[128] = {'\0'};
  strcpy(dest, log);
  int destlen = strlen(dest);
  char * fdest_file = dest;
  fdest_file = fdest_file + (char) destlen - (char)12;
  memset(fdest_file, '\0', 12);
  char destname[16] = "ftrace.txt";
  strcat(dest, destname);
  init_ftrace(ftrace_file, dest);

  /* Open the ringbuf file. */
  char ring[128] = {'\0'};
  strcpy(ring, log);
  int ringlen = strlen(ring);
  char * ring_file = ring; 
  ring_file = ring_file + (char) ringlen - (char)12;
  memset(ring_file, '\0', 12);
  char ringname[16] = "iringbuf.txt";
  strcat(ring, ringname);
//  printf("%s\n", ring);
  init_ring(ring);

  char mtracebuf[128] = {'\0'};
  strcpy(mtracebuf, log);
  int mtracelen = strlen(mtracebuf);
  char * mtrace_file = mtracebuf;
  mtrace_file = mtrace_file + (char) mtracelen - (char)12;
  memset(mtrace_file, '\0', 12);
  char mtracename[16] = "mtrace.txt";
  strcat(mtracebuf, mtracename);
//  printf("%s\n", ring);
  init_memtrace(mtracebuf);

  char dtracebuf[128] = {'\0'};
  strcpy(dtracebuf, log);
  int dtracelen = strlen(dtracebuf);
  char * dtrace_file = dtracebuf;
  dtrace_file = dtrace_file + (char) dtracelen - (char)12;
  memset(dtrace_file, '\0', 12);
  char dtracename[16] = "dtrace.txt";
  strcat(dtracebuf, dtracename);
//  printf("%s\n", ring);
  init_devtrace(dtracebuf);
#endif

  /* Initialize memory. */
  init_mem();

  /* Initialize devices. */
  IFDEF(CONFIG_DEVICE, init_device());

  /* Perform ISA dependent initialization. */
  init_isa();

  /* Load the image to memory. This will overwrite the built-in image. */
  long img_size = load_img();

  /* Initialize differential testing. */
  init_difftest(diff_so_file, img_size, difftest_port);

  /* Initialize the simple debugger. */
  init_sdb();

  IFDEF(CONFIG_ITRACE, init_disasm(
    MUXDEF(CONFIG_ISA_x86,     "i686",
    MUXDEF(CONFIG_ISA_mips32,  "mipsel",
    MUXDEF(CONFIG_ISA_riscv32, "riscv32",
    MUXDEF(CONFIG_ISA_riscv64, "riscv64", "bad")))) "-pc-linux-gnu"
  ));

  /* Display welcome message. */
  welcome();
}
#else // CONFIG_TARGET_AM
static long load_img() {
  extern char bin_start, bin_end;
  size_t size = &bin_end - &bin_start;
  Log("img size = %ld", size);
  memcpy(guest_to_host(RESET_VECTOR), &bin_start, size);
  return size;
}

void am_init_monitor() {
  init_rand();
  init_mem();
  init_isa();
  load_img();
  IFDEF(CONFIG_DEVICE, init_device());
  welcome();
}
#endif
