#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
//#include <ndl.h>

unsigned int last;
extern uint32_t NDL_GetTicks();

int main(){
	struct timeval tv;
	int i = 0;
	while(1){
//		gettimeofday(&tv, NULL);
//		if(tv.tv_usec < last || tv.tv_usec - last > 500000){
                if(NDL_GetTicks() / 500 > i){ 
			printf("%d times 0.5s passed, msec = %d!\n", i + 1, NDL_GetTicks());
			i++;
//			last = tv.tv_usec;
		}
	}
	return 0;
}
