#include<stdio.h>
#include<omp.h>

int fib(int n) {
  if (n < 2) return 1;
  int x = fib(n-1);
  int y = fib(n-2);
  return x + y;
}

int main() {
  #pragma omp parallel
  {
    int rank = omp_get_thread_num();
    int size = omp_get_num_threads();
    #pragma omp sections
    {
      #pragma omp section
      printf("rank %d; size %d\n", rank, size);
      #pragma omp section
      printf("rank %d; size %d\n", rank, size);
    }
  }
  printf("%d\n", fib(10));
  return 0;
}
