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
      x = fib_par(n-1);
      #pragma omp section
      y = fib_par(n-2);
    }
  }
  return x + y;
}

// nt - number of times it can parallelize
int fib_par_fixed(int n, int nt) {
  if (n < 2) return 1;
  unsigned long long int x, y;
  if (nt < 2) return fib(n);
  #pragma omp parallel num_threads(2)
  {
    #pragma omp sections
    {
      #pragma omp section
      x = fib_par_fixed(n-1, nt/2);
      #pragma omp section
      y = fib_par_fixed(n-2, nt - nt/2);
    }
  }
  return x + y;
}

int fib_par_nested(int n, int nt) {
  if (n < 2) return 1;
  unsigned long long int x, y;
  if (nt < 2) return fib(n);
  #pragma omp parallel num_threads(2)
  {
    #pragma omp sections
    {
      #pragma omp section
      x = fib_par_nested(n-1, nt/2);
      #pragma omp section
      y = fib_par_nested(n-2, nt - nt/2);
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
  int nt = omp_get_max_threads();
  // this flag is actually needed
  omp_set_nested(1);
  int res = fib_par_fixed(n, nt);
  //int res = fib_par(n);
  printf("%d\n", res);
  return 0;
}
