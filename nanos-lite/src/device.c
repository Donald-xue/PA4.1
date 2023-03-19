#include <common.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

size_t serial_write(const void *buf, size_t offset, size_t len) {
	yield();
	char * str = (char *)buf;
	for(int i = 0; i < len; i++){
            putch(*str);
            str++;
	}
	return len;
}

size_t events_read(void *buf, size_t offset, size_t len) {
//	int key = io_read(AM_INPUT_KEYBRD).keycode;
//	printf("key = %d\n", key);
    yield();
	AM_INPUT_KEYBRD_T kbd;
	kbd = io_read(AM_INPUT_KEYBRD);
//	printf("key = %d\n", kbd.keycode);
	char *du = buf;
	if(kbd.keycode == AM_KEY_NONE){
		return 0;
	}
	else{
//		printf("Not KEY_NONE key = %s\n", keyname[kbd.keycode]);
		if(kbd.keydown){
			*du = 'k';
			*(du+1) = 'd';
			*(du+2) = '\0';
		}
		else{
			*du = 'k'; 
			*(du+1) = 'u'; 
			*(du+2) = '\0';
		}
		size_t real = sprintf(buf, "%s %s\n", du, keyname[kbd.keycode]);
	//	printf("%s\n", keyname[kbd.keycode]);
		if(real > len)
			panic("real byte > max len!\n");
//		printf("Not KEY_NONE key = %s\n", buf);
		return real;
	}
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
	int width = io_read(AM_GPU_CONFIG).width;
	int height = io_read(AM_GPU_CONFIG).height;
//	printf("WIDTH:%d\nHEIGHT:%d\n", width, height);
	char mid[64];
	char *b, *m;
	b = (char *)buf;
	m = mid;
	sprintf(mid, "WIDTH:%d\nHEIGHT:%d\n", width, height);
	for(int i = 0; i < len && *m != '\0'; i++){
		*b = *m;
		b++; m++;
	}
//	printf("WIDTH:%d\nHEIGHT:%d\n", width, height);
	return len;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
	yield();
//	printf("offset = %d, len = %d\n", offset, len);
	int x,y;
	int w = io_read(AM_GPU_CONFIG).width;
	offset = offset / 4;
	x = offset % w;
	y = offset / w;
	int count = len / 4;
//	if(count > w) count = w;
	io_write(AM_GPU_FBDRAW, x, y, (void *)buf, count, 1, true);
	return len;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
