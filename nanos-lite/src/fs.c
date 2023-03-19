#include <fs.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

extern size_t ramdisk_read(void *buf, size_t offset, size_t len);
extern size_t ramdisk_write(const void *buf, size_t offset, size_t len);
extern size_t serial_write(const void *buf, size_t offset, size_t len);
extern size_t events_read(void *buf, size_t offset, size_t len);
extern size_t dispinfo_read(void *buf, size_t offset, size_t len);
extern size_t fb_write(const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  size_t open_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, 0, invalid_read, serial_write},
  [FD_STDERR] = {"stderr", 0, 0, 0, invalid_read, serial_write},
  #include "files.h"
  {"/dev/fb", 0, 0, 0, invalid_read, fb_write},
  {"/proc/dispinfo", 64, 0, 0, dispinfo_read, invalid_write},
  {"/dev/events", 0, 0, 0, events_read, invalid_write},
};

int NR_FILES = (sizeof(file_table) / sizeof(file_table[0]));

void init_fs() {
  // TODO: initialize the size of /dev/fb
     for (int i = 0; i < NR_FILES; ++i) {
		 if (strcmp(file_table[i].name, "/dev/fb") == 0) {
			 file_table[i].size = io_read(AM_GPU_CONFIG).width * io_read(AM_GPU_CONFIG).height * 4;
		 }
	 }
}

int fs_open(const char *pathname){
//	char fn[32] = "/bin/pal";
//	printf("fs_open filename %s\n", pathname);
	for(int i = 0; i < NR_FILES; i++){
//		if (strcmp(pathname, fn) == 0)
//		printf("file%d %s\n", i, file_table[i].name);
		if(strcmp(pathname, file_table[i].name) == 0){
			file_table[i].open_offset = 0;
	//		printf("open file %s\n", pathname);
			return i;
		}
	}
	printf("File %s don't exist!\n", pathname);
	return -1;
}

size_t fs_read(int fd, void *buf, size_t len){
	if(fd == NR_FILES - 1){
//		printf("KEY return = %d!\n", events_read(buf, 0, len));
		return events_read(buf, 0, len);
	}
	if(fd == NR_FILES - 2){
		return dispinfo_read(buf, 64, len);
	}
	if(file_table[fd].open_offset + len > file_table[fd].size){
		len = file_table[fd].size - file_table[fd].open_offset;
	}
	size_t ret;
	ret = ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
	file_table[fd].open_offset += len;
	return ret;
}

size_t fs_write(int fd, const void *buf, size_t len){
	if(fd == 1 || fd == 2){
		return file_table[fd].write(buf, 0, len);
	}else if(fd == NR_FILES - 3){
		int ret = fb_write(buf, file_table[fd].open_offset, len);
		file_table[fd].open_offset += len;
		return ret;
	}else{
		if(file_table[fd].open_offset + len > file_table[fd].size){
        len = file_table[fd].size - file_table[fd].open_offset;
		}
		size_t ret;
		ret = ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
		file_table[fd].open_offset += len;
		return ret;
	}
}


size_t fs_lseek(int fd, size_t offset, int whence){
	switch(whence){
		case SEEK_SET:
		    if(offset < 0)
			return -1;	
//			if(offset > file_table[fd].size){
//				offset = file_table[fd].size;
//			}
			file_table[fd].open_offset = offset;
			break;
		case SEEK_CUR:
			file_table[fd].open_offset += offset;
			break;
		case SEEK_END:
			file_table[fd].open_offset = file_table[fd].size;
			file_table[fd].open_offset += offset;
			break;
	}
	return file_table[fd].open_offset;
}

void fs_setoffset(int fd, size_t offset){
	if(offset > file_table[fd].size){
        offset = file_table[fd].size;
    }
	file_table[fd].open_offset = offset;
}

size_t fs_getdiskoff(int fd){
	return file_table[fd].disk_offset;
}
