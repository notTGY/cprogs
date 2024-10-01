#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include <time.h>
#include <stdint.h>
#include <inttypes.h>


int power(int n) {
  int res = 1;
  while (n-- > 0) {
    res *= 10;
  }
  return res;
}
int order(int n) {
  int res = 0;
  while (n / power(res+1) > 0) {
    res++;
  }
  return res;
}

int main(int argc, char **argv) {
  int i, rank, size;
  MPI_Status status;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (size < 2) {
    printf("This program requires at least 2 processes");
    MPI_Finalize();
    return 1;
  }


  struct timespec tms;
  int other_rank = rank == 0 ? 1 : 0;
  int payload_size, payload_order, roundtrips_order, n_roundtrips;
  char* buf;

  payload_size = 0;

  double sum_t = 0;
  double sum_m = 0;
  double sum_tm = 0;
  double sum_mm = 0;
  int n = 0;

  while (payload_size < power(6)) {
    n++;
    payload_order = order(payload_size);
    int inc_order = payload_order;
    payload_size += power(inc_order);



    buf = (char*)malloc(sizeof(char) * payload_size);
    if (!buf) {
      printf("failed to allocate %d bytes", payload_size);
      MPI_Finalize();
      return 1;
    }
    for (int i = 0; i < payload_size; i++) {
      buf[i] = 0;
    }

    roundtrips_order = (5 - payload_order) > 4 ? 5 - payload_order : 4;
    n_roundtrips = power(roundtrips_order);


    if (rank == 0) {
      MPI_Recv(buf, payload_size, MPI_CHAR, other_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

      if (clock_gettime(CLOCK_REALTIME, &tms)) {
        printf("Failed to get time");
        free(buf);
        MPI_Finalize();
        return -1;
      }
      int64_t start = tms.tv_sec * 1000000;
      start += tms.tv_nsec/1000;

      for (int i = 0; i < n_roundtrips; i++) {
        MPI_Send(buf, payload_size, MPI_CHAR, other_rank, 0, MPI_COMM_WORLD);
        MPI_Recv(buf, payload_size, MPI_CHAR, other_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      }

      if (clock_gettime(CLOCK_REALTIME, &tms)) {
        printf("Failed to get time");
        free(buf);
        MPI_Finalize();
        return -1;
      }
      int64_t end = tms.tv_sec * 1000000;
      end += tms.tv_nsec/1000;
      
      double dt = (double)(end - start) / (double)n_roundtrips;

      sum_t += dt;
      sum_m += (double)payload_size;
      sum_tm += dt * (double)payload_size;
      sum_mm += (double)payload_size * (double)payload_size;

      /*
      printf(
        "completed %d roundtrips with %d bytes in %ld mcs %.6lf avg\n",
        n_roundtrips,
        payload_size,
        end - start,
        dt
      );
      */
      printf(
        "[%d,\t%.6lf],\n",
        payload_size,
        dt
      );
    } else {
      MPI_Send(buf, payload_size, MPI_CHAR, other_rank, 0, MPI_COMM_WORLD);

      for (int i = 0; i < n_roundtrips; i++) {
        MPI_Recv(buf, payload_size, MPI_CHAR, other_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Send(buf, payload_size, MPI_CHAR, other_rank, 0, MPI_COMM_WORLD);
      }
    }
    free(buf);
  }

  if (rank == 0) {
    double beta = ((double)n * sum_tm - sum_t * sum_m) / ((double)n * sum_mm - sum_m * sum_m);
    double alpha = (sum_t - beta * sum_m) / (double)n;
    printf("\nalpha: %lf; beta: %lf\n", alpha, beta);
  }



  MPI_Finalize();
  return 0;
}
