#include<stdio.h>
#include<stdlib.h>
#include<omp.h>
#include<time.h>

void printArr(int* a, int n) {
  for (int i = 0; i < n; i++) {
    printf("%d ", a[i]);
  }
  printf("\n");
}


void merge(int* r, int* f, int* s, int nf, int ns) {
  //printf("%d %d\n", nf, ns);
  //printArr(f, nf);
  //printArr(s, ns);
  int n = nf+ns;
  int pf = 0;
  int ps = 0;
  int i = 0;
  while (i < n) {
    //printf("%d %d %d [%d] [%d]\n", i, pf, ps, f[pf], s[ps]);
    if (pf >= nf) {
      r[i++] = s[ps++];
      continue;
    }
    if (ps >= ns) {
      r[i++] = f[pf++];
      continue;
    }
    if (f[pf] < s[ps]) {
      r[i++] = f[pf++];
    } else {
      r[i++] = s[ps++];
    }
  }
  //printf("%d %d %d [%d] [%d]\n", i, pf, ps, f[pf], s[ps]);
  //printArr(r, n);
  //printf("\n");
  return;
}

void merge_sort(int* r, int* a, int n, int nt) {
  if (n < 2) {
    for (int i = 0; i < n; i++) {
      r[i] = a[i];
    }
    return;
  }
  int nf = n/2;
  int ns = n-n/2;
  int *f = (int*)malloc(nf*sizeof(int));
  int *s = (int*)malloc(ns*sizeof(int));
  if (nt < 2) {
    merge_sort(f, a, nf, 0);
    merge_sort(s, a + nf, ns, 0);
    merge(r, f, s, nf, ns);
    free(f);
    free(s);
    return;
  }
  #pragma omp parallel num_threads(2)
  {
    #pragma omp sections
    {
      #pragma omp section
      merge_sort(f, a, nf, nt/2);
      #pragma omp section
      merge_sort(s, a + nf, ns, nt-nt/2);
    }
  }
  merge(r, f, s, nf, ns);
  free(f);
  free(s);
  return;
}

int main(int argc, char* *argv) {
  srand(time(NULL));
  if (argc < 2) {
    printf("usage: %s 30\n", argv[0]);
    return 1;
  }
  int n = atoi(argv[1]);

  int* a = (int*) malloc(n * sizeof(int));
  for (int i = 0; i < n; i++) {
    a[i] = rand() % n;
  }

  int nt = omp_get_max_threads();
  // this flag is actually needed
  omp_set_nested(1);
  nt = 0;

  int* sorted = (int*)malloc(n*sizeof(int));
  merge_sort(sorted, a, n, nt);
  printArr(sorted, n);

  free(a);
  free(sorted);
  return 0;
}
