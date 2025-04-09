#include<stdio.h>
#include<stdlib.h>
#include <mpi.h>

int fib(int n) {
  if (n < 2) return 1;
  unsigned long long int x, y;
  x = fib(n-1);
  y = fib(n-2);
  return x + y;
}

int main(int argc, char* *argv) {
	MPI_Init(&argc, &argv);

  int n = 20;
  int res = fib(n);
  printf("%d\n", res);

  MPI_Finalize();
  return 0;
}
