#include<stdio.h>
#include<stdlib.h>
#include<omp.h>

int fib(int n) {
  if (n < 2) return 1;
  unsigned long long int x, y;
  x = fib(n-1);
  y = fib(n-2);
  return x + y;
}

int fib_par(int n) {
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

int main(int argc, char* *argv) {
  if (argc < 2) {
    printf("usage: %s 30\n", argv[0]);
    return 1;
  }
  int n = atoi(argv[1]);
  printf("%d\n", fib(n));
  return 0;
}
