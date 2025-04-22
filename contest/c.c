#include<stdio.h>
#include<stdlib.h>

int main() {
  int N;
  scanf("%d", &N);
  int *cost = (int*)malloc(sizeof(int) * N);
  int *C = (int*)malloc(sizeof(int) * N * N);

  for (int i = 0; i < N*N; i++) {
    scanf("%d", &(C[i]));
  }
  for (int i = 0; i < N; i++) {
    scanf("%d", &(cost[i]));
  }

  int res = 0;
  int offers = 0;
  int minOffer = cost[0];
  for (int i = 0; i < N; i++) {
    int minC = cost[i];
    for (int j = 0; j < N; j++) {
      if (i == j) continue;
      if (minC < C[i*N+j]) {
        minC = C[i*N+j];
      }
    }
    res += minC;

    
  }
  if (offers == 0) {
    
  }
  printf("%d", res);

  free(C);
  free(cost);
  return 0;
}
