#include<stdio.h>
#include<stdlib.h>

int main() {
  int n, m, k;
  scanf("%d", &n);
  int *A = (int*)malloc(sizeof(int) * n);
  for (int i = 0; i < n; i++) {
    scanf("%d", &(A[i]));
  }
  scanf("%d", &m);
  int *B = (int*)malloc(sizeof(int) * m);
  for (int i = 0; i < m; i++) {
    scanf("%d", &(B[i]));
  }
  scanf("%d", &k);

  int res = 0;

  int ai = 0;
  int bi = m-1;
  while (ai < n) {
    while (B[bi] + A[ai] > k && bi >= 0) {
      bi--;
    }
    if (B[bi] + A[ai] == k) {
      res++;
    }
    ai++;
  }

  printf("%d", res);
  return 0;
}
