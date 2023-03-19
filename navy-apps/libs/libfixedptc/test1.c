#include<stdio.h>
#include<math.h>

int main(){
    double a = 2;
    int N = 10;//我们取了前10+1项来估算
   int k,nk;
   double x,xx,y;
   x = (a-1)/(a+1);
   xx = x*x;
   nk = 2*N+1;
   y = 1.0/nk;
   for(k=N;k>0;k--)
   {
     nk = nk - 2;
     y = 1.0/nk+xx*y;
   }
   double b = 2.0*x*y;
   double c = log(2);
//   printf("b = %f, c = %f\n",b, c);
   b *= 256;
   int d = (int)b;
   printf("%d\n", d);
   return 0;
}