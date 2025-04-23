#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>

void printArr(int* a, int n) {
  for (int i = 0; i < n; i++) {
    printf("%d ", a[i]);
  }
  printf("\n");
}

void merge(int* res, int* f, int* s, int nf, int ns) {
  int n = nf+ns;
  int* buf = malloc(n*sizeof(int));
  int pf = 0;
  int ps = 0;
  int i = 0;
  while (i < n) {
    if (pf >= nf) {
      buf[i++] = s[ps++];
      continue;
    }
    if (ps >= ns) {
      buf[i++] = f[pf++];
      continue;
    }
    buf[i++] = f[pf] < s[ps] ? f[pf++] : s[ps++];
  }
  for (int i = 0; i < n; i++) {
    res[i] = buf[i];
  }
  free(buf);
  return;
}

void merge_sort(int* a, int n) {
  if (n < 2) {
    return;
  }
  merge_sort(a, n/2);
  merge_sort(a + n/2, n-n/2);
  merge(a, a, a+n/2, n/2, n-n/2);
  return;
}

int my_log2(int n) {
  if (n == 1) {
    return 0;
  } else if (n == 2) {
    return 1;
  } else if (n == 4) {
    return 2;
  } else if (n == 8) {
    return 3;
  } else if (n == 16) {
    return 4;
  }
  return -1;
}

void merge_sort_parallel(int* a, int n, int delve, int id) {
  if (n < 2) {
    return;
  }
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (delve == 0) {
    if (rank != id) {
      return;
    }
    merge_sort(a, n);
  } else {
    merge_sort_parallel(a, n/2, delve-1, id*2);
    merge_sort_parallel(a + n/2, n-n/2, delve-1, id*2+1);
  }
  return;
}

int main(int argc, char* *argv) {
  int size, rank, delve;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  delve = my_log2(size);
  if (delve == -1) {
    printf("The only valid -np={1,2,4,8,16}\n");
    MPI_Finalize();
    return 1;
  }
  if (argc < 2) {
    printf("usage: %s 100000000\n", argv[0]);
    MPI_Finalize();
    return 1;
  }
  int n = atoi(argv[1]);
  if (n % size != 0) {
    printf("Kindly provide n divisible by np\n");
    MPI_Finalize();
    return 1;
  }

  int* a = (int*) malloc(n * sizeof(int));
  for (int i = 0; i < n; i++) {
    a[i] = n - i;
  }


  merge_sort_parallel(a, n, delve, 0);

  int nk = n / size;
  int start = nk * rank;
  int d = delve;
  int k = rank;
  int mask = 0;
  int i2 = 1;
  for (int i = 0; i < d; i++) {
    if ((rank & mask) == 0) {
      int partner = k ^ i2;
      if ((k & i2) != 0) {
        MPI_Send(a+start, nk, MPI_INT, partner, 0, MPI_COMM_WORLD);
      } else {
        MPI_Recv(a+start+nk, nk, MPI_INT, partner, 0, MPI_COMM_WORLD, NULL);
        merge(a+start, a+start, a+start+nk, nk, nk);
        nk*=2;
      }
    }
    mask = mask ^ i2;

    i2 *= 2;
  }


  /*
  if (rank == 0) {
    printArr(a, n);
  }
  */

  free(a);
  MPI_Finalize();
  return 0;
}
