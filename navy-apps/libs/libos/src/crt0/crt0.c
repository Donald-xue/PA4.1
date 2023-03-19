#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args) {
  uintptr_t argc = *args;
  char **argv = (char **)(args + 1);
  for (args += 1; *args; ++args){}
  char **envp = (char **)(args + 1);
//  char **envp = argv;
//  while(*envp)  envp++;
//  ++envp;
//  char *empty[] =  {NULL };
  environ = envp;
  exit(main(argc, argv, envp));
  assert(0);
}
