#include<stdio.h>
#include<stdlib.h>
#include<omp.h>

void printArr(int* a, int n) {
  for (int i = 0; i < n; i++) {
    printf("%d ", a[i]);
  }
  printf("\n");
}
void merge(int* res, int* f, int* s, int nf, int ns) {
  //printf("%d %d\n", nf, ns);
  //printArr(f, nf);
  //printArr(s, ns);
  int n = nf+ns;
  int* buf = malloc(n*sizeof(int));
  int pf = 0;
  int ps = 0;
  int i = 0;
  while (i < n) {
    //printf("%d %d %d [%d] [%d]\n", i, pf, ps, f[pf], s[ps]);
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
  //printf("%d %d %d [%d] [%d]\n", i, pf, ps, f[pf], s[ps]);
  //printArr(r, n);
  //printf("\n");
  return;
}
void merge_sort(int* a, int n) {
  if (n < 2) {
    return;
  }
  if (n < 2500000) {
    merge_sort(a, n/2);
    merge_sort(a + n/2, n-n/2);
    merge(a, a, a+n/2, n/2, n-n/2);
    return;
  }
  int i, j;
#pragma omp task default(none) shared(i) firstprivate(n, a)
  {
    merge_sort(a, n/2);
    i = 0;
  }
#pragma omp task default(none) shared(j) firstprivate(n, a)
  {
    merge_sort(a + n/2, n-n/2);
    j = 0;
  }

#pragma omp taskwait
  merge(a, a, a+n/2, n/2, n-n/2);
  return;
}

int main(int argc, char* *argv) {
  if (argc < 2) {
    printf("usage: %s 30\n", argv[0]);
    return 1;
  }
  int n = atoi(argv[1]);

  int* a = (int*) malloc(n * sizeof(int));
  for (int i = 0; i < n; i++) {
    a[i] = n - i;
  }

  omp_set_num_threads(8);
  //printArr(a, n);

  #pragma omp parallel
  #pragma omp single
  merge_sort(a, n);

  //printArr(a, n);

  free(a);
  return 0;
}
