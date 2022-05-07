#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
	int len = 0;
	const char *ret = s;
	while((*ret++) != '\0'){
		len++;
	}
	return len;
// panic("Not implemented");
}

char *strcpy(char *dst, const char *src) {
	size_t i;

    for (i = 0; src[i] != '\0'; i++)
		dst[i] = src[i];
	dst[i] = '\0';

    return dst;
//  panic("Not implemented");
}

char *strncpy(char *dst, const char *src, size_t n) {
	size_t i;

    for (i = 0; i < n && src[i] != '\0'; i++)
		dst[i] = src[i];
    for ( ; i < n; i++)
        dst[i] = '\0';

    return dst;
//  panic("Not implemented");
}

char *strcat(char *dst, const char *src) {
	size_t dest_len = strlen(dst);
    size_t i;
    for (i = 0 ; src[i] != '\0' ; i++)
        dst[dest_len + i] = src[i];
    dst[dest_len + i] = '\0';

    return dst;
//  panic("Not implemented");
}

int strcmp(const char *s1, const char *s2) {
	while ((*s1) && (*s2) && (*s1 == *s2))
	{
		s1++;
		s2++;
	}

	if (*(unsigned char*)s1 > *(unsigned char*)s2)
	{
		return 1;
	}
	else if (*(unsigned char*)s1 < *(unsigned char*)s2)
	{
		return -1;
	}
	else
	{
		return 0;
	}
//  panic("Not implemented");
}

int strncmp(const char *s1, const char *s2, size_t n) {
	if ( !n )//n为无符号整形变量;如果n为0,则返回0
		return 0;
	while (n-- && (*s1) && (*s2) && (*s1 == *s2))
    {
        s1++;
        s2++;
    }

    if (*(unsigned char*)s1 > *(unsigned char*)s2)
    {
        return 1;
    }
    else if (*(unsigned char*)s1 < *(unsigned char*)s2)
    {
        return -1;
    }
    else
    {
        return 0;
    }
//  panic("Not implemented");
}

void *memset(void *s, int c, size_t n) {
	void* ret = s;
    while(n--)
    {
        *(char*)s = (char)c;
        s = (char*)s + 1; //移动一个字节
    }
    return ret;
//  panic("Not implemented");
}

void *memmove(void *dst, const void *src, size_t count) {
   void * ret = dst;
   if (dst <= src || (char *)dst >= ((char *)src + count))
   {  // 若dst和src区域没有重叠，则从起始处开始逐一拷贝
      while (count--)
      {
         *(char *)dst = *(char *)src;
         dst = (char *)dst + 1;
         src = (char *)src + 1;
      }
   }
   else
   {  // 若dst和src 区域交叉，则从尾部开始向起始位置拷贝，这样可以避免数据冲突
      dst = (char *)dst + count - 1;
      src = (char *)src + count - 1;
      while (count--)
      {
         *(char *)dst = *(char *)src;
         dst = (char *)dst - 1;
         src = (char *)src - 1;
      }
   }
   return(ret);
//  panic("Not implemented");
}

void *memcpy(void *out, const void *in, size_t n) {
   void * ret = out;
   while (n--)
   {  // 注意， memcpy函数没有处理dst和src区域是否重叠的问题
      *(char *)out = *(char *)in;
      out = (char *)out + 1;
      in = (char *)in + 1;
   }
   return(ret);
//  panic("Not implemented");
}

int memcmp(const void *s1, const void *s2, size_t n) {
   if (!n)
      return(0);

   while ( --n && *(char *)s1 == *(char *)s2)
   {
      s1 = (char *)s1 + 1;
      s2 = (char *)s2 + 1;
   }

   return( *((unsigned char *)s1) - *((unsigned char *)s2) );

/*	assert((NULL != s1) && (NULL != s2));

	const char *pstr1 = (const char*)s1;
	const char *pstr2 = (const char*)s2;

	while (n--) {
	    if (*pstr1 && *pstr2 && (*pstr1 == *pstr2)) {
			continue;
	    } else {
		break;
	    }
	}
	return (*pstr1 - *pstr2);*/
//  panic("Not implemented");
}

#endif
