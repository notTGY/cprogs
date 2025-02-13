#include<stdio.h>
#include<omp.h>

int fib(int n) {
  if (n < 2) return 1;
  unsigned long long int x, y;
  #pragma omp parallel
  {
    #pragma omp sections
    {
      #pragma omp section
      x = fib(n-1);
      #pragma omp section
      y = fib(n-2);
    }
  }
  return x + y;
}

int main() {
  printf("%d\n", fib(30));
  return 0;
}
