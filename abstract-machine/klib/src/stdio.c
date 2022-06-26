#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

extern void putch(char ch);

char hex_tab[]={'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
int pnum = 0;

int i2a(int num, char buf[32], int radix)
{
	static const char s[] = "0123456789abcdef";
	int n = num, R = radix;
	char *dst = buf;
	if (n < 0) { *dst++ = '-'; n = -n; }
	if (n < 10)
	{
		*dst++ = s[n]; *dst = '\0';
	}else
	{ 
		char tmp[32], *p = tmp;
		while (n) { *p++ = s[n % R]; n /= R; }
		while (--p != tmp) *dst++ = *p;
		*dst++ = *tmp; *dst = '\0';
	}
	return dst-buf;
}

static int outc(int c) 
{
	pnum++;
    putch(c);
    return 0;
}

static int outs (const char *s)
{
    while (*s != '\0'){    
        putch(*s++);
		pnum++;
	}
    return 0;
}

static int out_num(long long n, int base,char lead,int maxwidth) 
{
    long long m=0;
    char buf[128], *s = buf + sizeof(buf);
    int count=0,i=0;
            

    *--s = '\0';
    
    if (n < 0){
        m = -n;
    }
    else{
        m = n;
    }
    
    while ((m / base) != 0){
        *--s = hex_tab[m%base];
        count++;
		m /= base;
    }
   
	if(m < base){
		*--s = hex_tab[m%base];
		count++;
	}

    if( maxwidth && count < maxwidth){
        for (i=maxwidth - count; i; i--)    
            *--s = lead;
}

    if (n < 0)
        *--s = '-';
    
    return outs(s);
}

int printf(const char *fmt, ...) {
	pnum = 0;
	va_list ap;
	const char * f = fmt;
//	char prt[256];
//	char *s = prt;
//	char * head = s;
	char lead=' ';
    int  maxwidth=0;

	va_start(ap, fmt);
	for(; *f != '\0'; f++)
     {
            if (*f != '%') {
                pnum++;
				outc(*f);
                continue;
            }
        lead=' ';
        maxwidth=0;
        
        //format : %08d, %8d,%d,%u,%x,%f,%c,%s 
            f++;
        if(*f == '0'){
            lead = '0';
            f++;    
        }
        
        while(*f >= '0' && *f <= '9'){
            maxwidth *=10;
            maxwidth += (*f - '0');
            f++;
        }
        
            switch (*f) {
        case 'd': out_num(va_arg(ap, int),          10,lead,maxwidth); break;
        case 'o': out_num(va_arg(ap, unsigned int),  8,lead,maxwidth); break;                
        case 'u': out_num(va_arg(ap, unsigned int), 10,lead,maxwidth); break;
        case 'x': out_num(va_arg(ap, unsigned int), 16,lead,maxwidth); break;
            case 'c': outc(va_arg(ap, int   )); break;        
            case 's': outs(va_arg(ap, char *)); break;
			case 'p': out_num(va_arg(ap, unsigned long int), 16,lead,maxwidth); break;				  
                
            default:{	
						outc(*f);
						assert(0);
					}
                break;
            }
    }
//    va_copy(another, ap);

/*	while (*f)
     {
        int n = 1;
        if ('%' != *f)
         {
            *s = *f;
        }else{
			++f;
            switch (*f)
             {
				 case '0':
					 {
						 lead = '0';
						 ++f;
						 int i = *f - '0';
						 for(; i > 0; i--){
							 memset()
						 }
						 break;
					 }
				case 's':// 字符串
					{
						const char *p = va_arg(ap, char*);
						n = strlen(p);
						//strcat(out, p);
						memcpy(s, p, n);
					}
					break;

				case 'd':// 整数
					{
						char buf[32];
						//const char * b = buf;
						n = i2a(va_arg(ap, int), buf, 10);
						//strcat(out, b);
						memcpy(s, buf, n);
					}
					break;

            default:
                {
                    assert(0);
                    va_end(ap);
                    //int x = vsprintf(dst, format, another);
                    va_end(another);
                    //return x;
                 }
                break;
            }
        }
        ++f;
        s += n;
    } 
    *s = 0;
	char ch;
	s = head;
	while((ch = *(s++)) != 0){
		putch(ch);
	}
	int num = s - head;*/
    va_end(ap);
 	return pnum;
//  panic("Not implemented");
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}

int sprintf(char *out, const char *fmt, ...) {
	va_list ap, another;
	char *s = out;
	const char * f = fmt;

	va_start(ap, fmt);
	va_copy(another, ap);

	while (*f)
	 {
		int n = 1;
		if ('%' != *f)
		{
			*s = *f;
		}else{
			++f;
			switch (*f)
			{
			case 's':// 字符串
				{
					const char *p = va_arg(ap, char*);
					n = strlen(p);
					//strcat(out, p);
					memcpy(s, p, n);
				}
				break;

			case 'd':// 整数
				{
					char buf[32];
					//const char * b = buf;
					n = i2a(va_arg(ap, int), buf, 10);
					//strcat(out, b);
					memcpy(s, buf, n);
				}
				break;

			default:
				{
					assert(0);
					va_end(ap);
					//int x = vsprintf(dst, format, another);
					va_end(another);
					//return x;
				}
				break;
	 		}
	 	}
		++f;
		s += n;
	}
	*s = 0;
	va_end(ap);
	return s-out;
//  panic("Not implemented");
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
