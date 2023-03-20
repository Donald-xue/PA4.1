#include <common.h>
#include "proc.h"
#include "fs.h"
#include "syscall.h"

extern int fs_open(char *pathname);
extern size_t fs_read(int fd, void *buf, size_t len);
extern size_t fs_write(int fd, const void *buf, size_t len);
extern size_t fs_lseek(int fd, size_t offset, int whence);
void naive_uload(PCB *pcb, const char *filename);
Context* schedule(Context *prev);
int execve(const char *filename, char *const argv[], char *const envp[]);
int fs_close(int fd);

typedef long      time_t;
typedef long      suseconds_t;

struct timeval {
               time_t      tv_sec;     /* seconds */
               suseconds_t tv_usec;    /* microseconds */
           };

struct timezone {
               int tz_minuteswest;     /* minutes west of Greenwich */
               int tz_dsttime;         /* type of DST correction */
           };

void sys_yield(Context *c){
	yield();
//    c->GPR1	= 0;
	c->GPRx = 0;
	//return 0;
}

int sys_write(int fd, char *buf, size_t count, Context *c){
/*	if(fd == 1 || fd == 2){
		for(int i = 0; i < count; i++){
			putch(*buf);
			buf++;
		}
		c->GPRx = count;
 		return count;
	}*/
// 	else if(fd > 2 && fd < 23){
 	size_t ret;
	ret = fs_write(fd, (void *)buf, count);
	c->GPRx = ret;
//	printf("In SYS_write, c->GPRx = %x\n", c->GPRx);
	return ret;
//	}
//	c->GPRx = -1;
//	return -1;
}

void *sys_brk(intptr_t pb, Context *c){
	c->GPRx = 0;
	return (void *) 0;
}

int sys_gettime(struct timeval *tv, struct timezone *tz){
	if(tz != NULL)
		panic("In gettime tz not NULL!\n");
//	AM_TIMER_RTC_T rtc;
//	rtc = io_read(AM_TIMER_RTC);
	uint64_t us = io_read(AM_TIMER_UPTIME).us;
//	printf("us = %u\n", us);
	uint32_t usec = us % 1000000;
	uint32_t sec = us / 1000000;
	tv->tv_usec = usec;
//	printf("us = %u\n", usec);
	tv->tv_sec = sec;
	return 0;
}

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  switch (a[0]) {
	  case 0: printf("Get an SYS_exit!\n");
			  //halt(c->GPRx);
			  c->GPRx = execve("/bin/nterm", (char **)a[2], (char **)a[3]);
//			  printf("In SYS_exit, c->GPRx = %x\n", c->GPRx);
			  //c->GPRx = execve("/bin/nterm", (char **)a[2], (char **)a[3]);
			  break;

	  case 1: printf("Get an SYS_yield!\n");
	//		  c = schedule(current->cp); 
	//		  printf("SYS_yield after schedule!\n");
			  sys_yield(c);
			  break;

	  case 2: //printf("Get an SYS_open!\n");
			  c->GPRx = fs_open((char *)a[1]);
//			  printf("In SYS_open, c->GPRx = %x\n", c->GPRx);
			  //fs_open((char *)a[1]);
			  break;

	  case 3: //printf("Get an SYS_read!\n");
			  c->GPRx = fs_read(a[1], (void *)a[2], a[3]);
//			  printf("In SYS_read, c->GPRx = %x\n", c->GPRx);
			  //printf("SYS_read GPRx = %d!\n", c->GPRx);
			  break;

	  case 4: //printf("Get an SYS_write!\n");
			  sys_write(a[1], (char *)a[2], a[3], c);
			  break;

	  case 7: //printf("Get an SYS_close!\n");
              c->GPRx = fs_close(c->GPR2);
			  break; 

	  case 8:// printf("Get an SYS_lseek!\n");
			  c->GPRx = fs_lseek(a[1], a[2], a[3]);
	//		  printf("In SYS_lseek, c->GPRx = %x\n", c->GPRx);
			  break;

	  case 9: //printf("Get an SYS_brk!\n");
			  sys_brk(a[1], c);
			  break;
		
	  case 13: 
		//	  printf("SYS_execve fliename %s, strlen %c\n", (char *)a[1], * ((char *)a[1] + strlen((char *)a[1]) - 1));
		//	  naive_uload(NULL, (char *)a[1]);
		      c->GPRx = execve((const char *)a[1], (char **)a[2], (char **)a[3]);
			  printf("In SYS_execve, c->GPRx = %x\n", c->GPRx);
			  break;

	  case 19: //printf("Get an SYS_gettimeofday!\n");
			   c->GPRx = sys_gettime((struct timeval *)a[1], (struct timezone *)a[2]);
			   break;

      default: panic("Unhandled syscall ID = %d", a[0]);
  }
//  printf("Finished do_syscall!\n");
}
