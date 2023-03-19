#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0, canvas_w = 0, canvas_h = 0, pad_x = 0, pad_y = 0;
static FILE* fb;
static FILE* evt;

static void get_dispinfo(){
	char w[64], h[64];
/*	FILE *fp = fopen("/proc/dispinfo", "r+");
	assert(fp);
	char w[64], h[64];
//	printf("w = %s, h = %s!\n", w, h);
	fgets(w, 64, fp);
	fgets(h, 64, fp);
	*/
//	printf("w = %s, h = %s!\n", w, h);
	int fd = open("/proc/dispinfo", O_RDWR);
	assert(fd != -1);
	read(fd, w, 64);
	read(fd, h, 64);
	char *p = w;
	while(*p != ':'){
//		printf("HERE!\n");
		p++;
	}
	p++;
	sscanf(p, "%d", &screen_w);
	p = h;
	while(*p != ':'){
        p++;
    }
	p++;
	sscanf(p, "%d", &screen_h);
//	fclose(fp);
	assert(screen_w > 0 && screen_h > 0);
//	printf("get_dispinfo screen_w = %d, screen_h = %d\n", screen_w, screen_h);
} 

uint32_t NDL_GetTicks() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	uint32_t ms = tv.tv_sec * 1000 + tv.tv_usec / 1000;
	return ms;
}

int NDL_PollEvent(char *buf, int len) {
	int fd = open("/dev/events", O_RDWR);
	assert(fd);
	char *ret = buf;
	while (1) {
      // 3 = evtdev
      int nread = read(fd, buf, len - 2);
      if (nread <= 0) continue;
      buf[nread] = '\0';
	  break;
    }
	buf[63] = '\n';
	if (buf[0] == ' ') len = -1;
	else{
		while(*ret != '\n'){
			ret++;
		}
		*ret = ' ';
	}
//	fclose(fp);
//	printf("key = %c, len = %d\n", buf[0], len);
	return len;	
}

int NDL_PollEvent_refresh(char *buf, int len) {
	int fd = open("/dev/events", O_RDWR);
//	FILE *fp = fopen("/dev/events", "r");
	assert(fd);
	char *ret = buf;
    // 3 = evtdev
    int nread = read(fd, buf, len - 2);
	if (nread <= 0) {
		//printf("NDL_PollEvent_refresh nread == 0\n");
		return nread;
	}
    buf[nread] = '\0';
	buf[63] = '\n';
	if (buf[0] == ' ') len = -1;
	else{
		while(*ret != '\n'){
			ret++;
		}
		*ret = ' ';
	}
//	fclose(fp);
//	printf("NDL_PollEvent_refresh key = %s, len = %d\n", buf, len);
	return len;	
}

void NDL_OpenCanvas(int *w, int *h) {
   get_dispinfo();
   if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
	if(*w != 0 || *h != 0) {
		screen_w = *w; 
		screen_h = *h;
	} else if (screen_w == 0 && screen_h == 0) {
		screen_w = 400;
		screen_h = 400;
	} else {}
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  }
  if(*w != 0 || *h != 0){
	  canvas_w = *w;
	  canvas_h = *h;
//	  screen_w = *w;
//	  screen_h = *h;
  }else if (canvas_w == 0 && canvas_h == 0){
	  canvas_w = screen_w;
	  canvas_h = screen_h;
	  *w = canvas_w;
	  *h = canvas_h;
  } else {
//	if(screen_w < canvas_w)
//	  screen_w = canvas_w;
//	if(screen_h < canvas_h)
//	  screen_h = canvas_h;
  }
  //printf("NDL_OpenCanvas canvas_w = %d, canvas_h = %d, screen_w = %d, screen_h = %d\n", canvas_w, canvas_h, screen_w, screen_h);
  assert(screen_w >= canvas_w);
  assert(screen_h >= canvas_h);
  pad_x = (screen_w - canvas_w) / 2;
  pad_y = (screen_h - canvas_h) / 2;
  fbdev = open("/dev/fb", O_RDWR);
  assert(fbdev);
  evtdev = open("/dev/events", O_RDWR);
  assert(evtdev);
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
	int offset = y * screen_w + x;
	offset = offset * 4;
	for(int i = 0; i < screen_h; i++){
	    //printf("In the NDL_DrawRect!\n");
		lseek(fbdev, ((i + pad_y) * screen_w + pad_x)*4, SEEK_SET);
		write(fbdev, &pixels[i * canvas_w], 4*canvas_w);
	}
//    for(int i = 0; i < screen_h; i++){
//        fseek(fb, ((i + y) * screen_w + x)*4, SEEK_SET);
//        fwrite(&pixels[i * canvas_w], 4, canvas_w, fb);
//    }
	fflush(stdout);
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  return 0;
}

void NDL_Quit() {
}
