#include<stdio.h>
#include<mpi.h>
#include<math.h>

double f(double x) {
  return sqrt(4 - x * x);
}

double sum(double a, double b, int N, int n_start, int n_end) {
  double res = 0;
  for (int i = n_start; i <= n_end; i++) {
     double x = a + ((double)i / (double)N) * (b - a);
     double f_x = f(x);
     res += f_x;
  }
  return res;
}

int main(int argc, char **argv) {
  int i, rank, size;
  MPI_Status status;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int N;
  sscanf(argv[1], "%d", &N);

  double a = 0.0;
  double b = 2.0;
  
  int n_start = 1+rank * (N/size);
  int n_end = (rank+1) * (N/size);
  if (rank == size - 1) {
    n_end = N;
  }
  double s = sum(a, b, N, n_start, n_end);
  double buf[1];

  if (rank == 0) {
    for (int i = 1; i < size; i++) {
      MPI_Recv(buf, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      s += buf[0];
    }
    double h = (b - a) / (double)N;
    double res = h * (f(a)/2 + f(b)/2 + s);
    printf("%.6lf\n", res);
  } else {
    buf[0] = s;
    MPI_Send(buf, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
  }
  MPI_Finalize();
  return 0;
}
